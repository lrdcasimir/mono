
/*
 * domain.c: MonoDomain functions
 *
 * Author:
 *	Dietmar Maurer (dietmar@ximian.com)
 *
 * (C) 2001 Ximian, Inc.
 */

#include <config.h>
#include <glib.h>
#include <string.h>

#if HAVE_BOEHM_GC
#include <gc/gc.h>
#endif

#include <mono/metadata/object.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/exception.h>
#include <mono/metadata/cil-coff.h>

static guint32 appdomain_thread_id = 0;

MonoDomain *mono_root_domain = NULL;

static MonoJitInfoTable *
mono_jit_info_table_new (void)
{
	return g_array_new (FALSE, FALSE, sizeof (gpointer));
}

static void
mono_jit_info_table_free (MonoJitInfoTable *table)
{
	g_array_free (table, TRUE);
}

static int
mono_jit_info_table_index (MonoJitInfoTable *table, char *addr)
{
	int left = 0, right = table->len;

	while (left < right) {
		int pos = (left + right) / 2;
		MonoJitInfo *ji = g_array_index (table, gpointer, pos);
		char *start = ji->code_start;
		char *end = start + ji->code_size;

		if (addr < start)
			right = pos;
		else if (addr >= end) 
			left = pos + 1;
		else
			return pos;
	}

	return left;
}

MonoJitInfo *
mono_jit_info_table_find (MonoDomain *domain, char *addr)
{
	MonoJitInfoTable *table = domain->jit_info_table;
	int left = 0, right = table->len;

	while (left < right) {
		int pos = (left + right) / 2;
		MonoJitInfo *ji = g_array_index (table, gpointer, pos);
		char *start = ji->code_start;
		char *end = start + ji->code_size;

		if (addr < start)
			right = pos;
		else if (addr >= end) 
			left = pos + 1;
		else
			return ji;
	}

	/* maybe irt is shared code, so we also search in the root domain */
	if (domain != mono_root_domain)
		return mono_jit_info_table_find (mono_root_domain, addr);

	return NULL;
}

void
mono_jit_info_table_add (MonoDomain *domain, MonoJitInfo *ji)
{
	MonoJitInfoTable *table = domain->jit_info_table;
	gpointer start = ji->code_start;
	int pos = mono_jit_info_table_index (table, start);

	g_array_insert_val (table, pos, ji);
}

static int
ldstr_hash (const char* str)
{
	guint len, h;
	const char *end;
	len = mono_metadata_decode_blob_size (str, &str) - 1;
	end = str + len;
	h = *str;
	/*
	 * FIXME: The distribution may not be so nice with lots of
	 * null chars in the string.
	 */
	for (str += 1; str < end; str++)
		h = (h << 5) - h + *str;
	return h;
}

static gboolean
ldstr_equal (const char *str1, const char *str2) {
	int len, len2;
	len = mono_metadata_decode_blob_size (str1, NULL) - 1;
	len2 = mono_metadata_decode_blob_size (str2, NULL) - 1;
	if (len != len2)
		return 0;
	return memcmp (str1, str2, len) == 0;
}

#if HAVE_BOEHM_GC
static void
domain_finalizer (void *obj, void *data) {
	g_print ("domain finalized\n");
}
#endif

MonoDomain *
mono_domain_create (void)
{
	MonoDomain *domain;

#if HAVE_BOEHM_GC
	domain = GC_malloc (sizeof (MonoDomain));
	GC_register_finalizer (domain, domain_finalizer, NULL, NULL, NULL);
#else
	domain = g_new0 (MonoDomain, 1);
#endif
	domain->domain = NULL;
	domain->setup = NULL;
	domain->friendly_name = NULL;

	domain->mp = mono_mempool_new ();
	domain->env = g_hash_table_new (g_str_hash, g_str_equal);
	domain->assemblies = g_hash_table_new (g_str_hash, g_str_equal);
	domain->class_vtable_hash = mono_g_hash_table_new (NULL, NULL);
	domain->proxy_vtable_hash = mono_g_hash_table_new (NULL, NULL);
	domain->static_data_hash = mono_g_hash_table_new (NULL, NULL);
	domain->jit_code_hash = g_hash_table_new (NULL, NULL);
	domain->ldstr_table = mono_g_hash_table_new ((GHashFunc)ldstr_hash, (GCompareFunc)ldstr_equal);
	domain->jit_info_table = mono_jit_info_table_new ();

	InitializeCriticalSection (&domain->lock);
	return domain;
}

/**
 * mono_init:
 * 
 * Creates the initial application domain and initializes the mono_defaults
 * structure.
 * This function is guaranteed to not run any IL code.
 *
 * Returns: the initial domain.
 */
MonoDomain *
mono_init (const char *filename)
{
	static MonoDomain *domain = NULL;
	MonoAssembly *ass;
	MonoImageOpenStatus status = MONO_IMAGE_OK;
	MonoAssemblyName corlib_aname;

	if (domain)
		g_assert_not_reached ();

	appdomain_thread_id = TlsAlloc ();

	domain = mono_domain_create ();
	mono_root_domain = domain;

	TlsSetValue (appdomain_thread_id, domain);

	/* find the corlib */
	corlib_aname.name = "corlib";
	ass = mono_assembly_load (&corlib_aname, NULL, &status);
	if ((status != MONO_IMAGE_OK) || (ass == NULL)) {
		switch (status){
		case MONO_IMAGE_ERROR_ERRNO:
			g_print ("The assembly corlib.dll was not found or could not be loaded.\n");
			g_print ("It should have been installed in the `%s' directory.\n", MONO_ASSEMBLIES);
			break;
		case MONO_IMAGE_IMAGE_INVALID:
			g_print ("The file %s/corlib.dll is an invalid CIL image\n", MONO_ASSEMBLIES);
			break;
		case MONO_IMAGE_MISSING_ASSEMBLYREF:
			g_print ("Minning assembly reference in %s/corlib.dll\n", MONO_ASSEMBLIES);
			break;
		case MONO_IMAGE_OK:
			/* to suppress compiler warning */
			break;
		}
		
		exit (1);
	}
	mono_defaults.corlib = ass->image;

	mono_defaults.object_class = mono_class_from_name (
                mono_defaults.corlib, "System", "Object");
	g_assert (mono_defaults.object_class != 0);

	mono_defaults.void_class = mono_class_from_name (
                mono_defaults.corlib, "System", "Void");
	g_assert (mono_defaults.void_class != 0);

	mono_defaults.boolean_class = mono_class_from_name (
                mono_defaults.corlib, "System", "Boolean");
	g_assert (mono_defaults.boolean_class != 0);

	mono_defaults.byte_class = mono_class_from_name (
                mono_defaults.corlib, "System", "Byte");
	g_assert (mono_defaults.byte_class != 0);

	mono_defaults.sbyte_class = mono_class_from_name (
                mono_defaults.corlib, "System", "SByte");
	g_assert (mono_defaults.sbyte_class != 0);

	mono_defaults.int16_class = mono_class_from_name (
                mono_defaults.corlib, "System", "Int16");
	g_assert (mono_defaults.int16_class != 0);

	mono_defaults.uint16_class = mono_class_from_name (
                mono_defaults.corlib, "System", "UInt16");
	g_assert (mono_defaults.uint16_class != 0);

	mono_defaults.int32_class = mono_class_from_name (
                mono_defaults.corlib, "System", "Int32");
	g_assert (mono_defaults.int32_class != 0);

	mono_defaults.uint32_class = mono_class_from_name (
                mono_defaults.corlib, "System", "UInt32");
	g_assert (mono_defaults.uint32_class != 0);

	mono_defaults.uint_class = mono_class_from_name (
                mono_defaults.corlib, "System", "UIntPtr");
	g_assert (mono_defaults.uint_class != 0);

	mono_defaults.int_class = mono_class_from_name (
                mono_defaults.corlib, "System", "IntPtr");
	g_assert (mono_defaults.int_class != 0);

	mono_defaults.int64_class = mono_class_from_name (
                mono_defaults.corlib, "System", "Int64");
	g_assert (mono_defaults.int64_class != 0);

	mono_defaults.uint64_class = mono_class_from_name (
                mono_defaults.corlib, "System", "UInt64");
	g_assert (mono_defaults.uint64_class != 0);

	mono_defaults.single_class = mono_class_from_name (
                mono_defaults.corlib, "System", "Single");
	g_assert (mono_defaults.single_class != 0);

	mono_defaults.double_class = mono_class_from_name (
                mono_defaults.corlib, "System", "Double");
	g_assert (mono_defaults.double_class != 0);

	mono_defaults.char_class = mono_class_from_name (
                mono_defaults.corlib, "System", "Char");
	g_assert (mono_defaults.char_class != 0);

	mono_defaults.string_class = mono_class_from_name (
                mono_defaults.corlib, "System", "String");
	g_assert (mono_defaults.string_class != 0);

	mono_defaults.enum_class = mono_class_from_name (
                mono_defaults.corlib, "System", "Enum");
	g_assert (mono_defaults.enum_class != 0);

	mono_defaults.array_class = mono_class_from_name (
                mono_defaults.corlib, "System", "Array");
	g_assert (mono_defaults.array_class != 0);

	mono_defaults.multicastdelegate_class = mono_class_from_name (
		mono_defaults.corlib, "System", "MulticastDelegate");
	g_assert (mono_defaults.multicastdelegate_class != 0 );

	mono_defaults.asyncresult_class = mono_class_from_name (
		mono_defaults.corlib, "System.Runtime.Remoting.Messaging", 
		"AsyncResult");
	g_assert (mono_defaults.asyncresult_class != 0 );

	mono_defaults.waithandle_class = mono_class_from_name (
		mono_defaults.corlib, "System.Threading", "WaitHandle");
	g_assert (mono_defaults.waithandle_class != 0 );

	mono_defaults.typehandle_class = mono_class_from_name (
                mono_defaults.corlib, "System", "RuntimeTypeHandle");
	g_assert (mono_defaults.typehandle_class != 0);

	mono_defaults.methodhandle_class = mono_class_from_name (
                mono_defaults.corlib, "System", "RuntimeMethodHandle");
	g_assert (mono_defaults.methodhandle_class != 0);

	mono_defaults.fieldhandle_class = mono_class_from_name (
                mono_defaults.corlib, "System", "RuntimeFieldHandle");
	g_assert (mono_defaults.fieldhandle_class != 0);

	mono_defaults.monotype_class = mono_class_from_name (
                mono_defaults.corlib, "System", "MonoType");
	g_assert (mono_defaults.monotype_class != 0);

	mono_defaults.exception_class = mono_class_from_name (
                mono_defaults.corlib, "System", "Exception");
	g_assert (mono_defaults.exception_class != 0);

	mono_defaults.thread_class = mono_class_from_name (
                mono_defaults.corlib, "System.Threading", "Thread");
	g_assert (mono_defaults.thread_class != 0);

	mono_defaults.appdomain_class = mono_class_from_name (
                mono_defaults.corlib, "System", "AppDomain");
	g_assert (mono_defaults.appdomain_class != 0);

	mono_defaults.transparent_proxy_class = mono_class_from_name (
                mono_defaults.corlib, "System.Runtime.Remoting.Proxies", "TransparentProxy");
	g_assert (mono_defaults.transparent_proxy_class != 0);

	mono_defaults.real_proxy_class = mono_class_from_name (
                mono_defaults.corlib, "System.Runtime.Remoting.Proxies", "RealProxy");
	g_assert (mono_defaults.real_proxy_class != 0);

	mono_defaults.mono_method_message_class = mono_class_from_name (
                mono_defaults.corlib, "System.Runtime.Remoting.Messaging", "MonoMethodMessage");
	g_assert (mono_defaults.mono_method_message_class != 0);

	mono_defaults.field_info_class = mono_class_from_name (
		mono_defaults.corlib, "System.Reflection", "FieldInfo");
	g_assert (mono_defaults.field_info_class != 0);

	domain->friendly_name = g_path_get_basename (filename);

	return domain;
}

/**
 * mono_domain_get:
 *
 * Returns the current domain.
 */
inline MonoDomain *
mono_domain_get ()
{
	return ((MonoDomain *)TlsGetValue (appdomain_thread_id));
}

/**
 * mono_domain_set:
 * @domain: the new domain
 *
 * Sets the current domain to @domain.
 */
inline void
mono_domain_set (MonoDomain *domain)
{
	TlsSetValue (appdomain_thread_id, domain);
}

/**
 * mono_domain_assembly_open:
 * @domain: the application domain
 * @name: file name of the assembly
 *
 * fixme: maybe we should integrate this with mono_assembly_open ??
 */
MonoAssembly *
mono_domain_assembly_open (MonoDomain *domain, char *name)
{
	MonoAssembly *ass, *tmp;
	int i;

	if ((ass = g_hash_table_lookup (domain->assemblies, name)))
		return ass;

	if (!(ass = mono_assembly_open (name, NULL)))
		return NULL;

	mono_domain_lock (domain);
	g_hash_table_insert (domain->assemblies, ass->aname.name, ass);
	mono_domain_unlock (domain);

	/* FIXME: maybe this must be recursive ? */
	for (i = 0; (tmp = ass->image->references [i]) != NULL; i++) {
		if (!g_hash_table_lookup (domain->assemblies, tmp->aname.name))
			g_hash_table_insert (domain->assemblies, tmp->aname.name, tmp);
	}

	return ass;
}

static void
remove_assembly (gpointer key, gpointer value, gpointer user_data)
{
	mono_assembly_close ((MonoAssembly *)value);
}

void
mono_domain_unload (MonoDomain *domain, gboolean force)
{
	if ((domain == mono_root_domain) && !force) {
		g_warning ("cant unload root domain");
		return;
	}

	g_free (domain->friendly_name);
	g_hash_table_foreach (domain->assemblies, remove_assembly, NULL);

	g_hash_table_destroy (domain->env);
	g_hash_table_destroy (domain->assemblies);
	mono_g_hash_table_destroy (domain->class_vtable_hash);
	mono_g_hash_table_destroy (domain->proxy_vtable_hash);
	mono_g_hash_table_destroy (domain->static_data_hash);
	g_hash_table_destroy (domain->jit_code_hash);
	mono_g_hash_table_destroy (domain->ldstr_table);
	mono_jit_info_table_free (domain->jit_info_table);
	mono_mempool_destroy (domain->mp);
	DeleteCriticalSection (&domain->lock);
	
	/* FIXME: anything else required ? */

#if HAVE_BOEHM_GC
#else
	g_free (domain);
#endif

	if ((domain == mono_root_domain))
		mono_root_domain = NULL;
}

