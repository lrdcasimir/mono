//
// System.Drawing.gdipFunctions.cs
//
// Author: 
// Alexandre Pigolkine (pigolkine@gmx.de)
//

using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;

namespace System.Drawing {
	/// <summary>
	/// GDI+ API Functions
	/// </summary>
	public class GDIPlus {
		
		#region gdiplus.dll functions

		// startup / shutdown
		[DllImport("gdiplus.dll")]
		static internal extern Status GdiplusStartup(ref ulong token, ref GdiplusStartupInput input, ref GdiplusStartupOutput output);
		[DllImport("gdiplus.dll")]
		static internal extern void GdiplusShutdown(ref ulong token);
		
		static ulong GdiPlusToken;
		static GDIPlus ()
		{
			GdiplusStartupInput input = GdiplusStartupInput.MakeGdiplusStartupInput();
			GdiplusStartupOutput output = GdiplusStartupOutput.MakeGdiplusStartupOutput();
			GdiplusStartup (ref GdiPlusToken, ref input, ref output);
		}

		// Memory functions
		[DllImport("gdiplus.dll")]
		static internal extern IntPtr GdipAlloc (int size);
		[DllImport("gdiplus.dll")]
		static internal extern void GdipFree (IntPtr ptr);

		
		// Brush functions
		[DllImport("gdiplus.dll")]
		static internal extern Status GdipCloneBrush (IntPtr brush, out IntPtr clonedBrush);
		[DllImport("gdiplus.dll")]
		static internal extern Status GdipDeleteBrush (IntPtr brush);
		
		// Solid brush functions
		[DllImport("gdiplus.dll")]
		static internal extern Status GdipCreateSolidFill (int color, out int brush);
		
		// Graphics functions
		[DllImport("gdiplus.dll")]
		static internal extern Status GdipCreateFromHDC(IntPtr hDC, out int graphics);
		[DllImport("gdiplus.dll")]
		static internal extern Status GdipDeleteGraphics(IntPtr graphics);
		[DllImport("gdiplus.dll")]
		static internal extern Status GdipRestoreGraphics(IntPtr graphics, uint graphicsState);
		[DllImport("gdiplus.dll")]
		static internal extern Status GdipSaveGraphics(IntPtr graphics, out uint state);
		[DllImport("gdiplus.dll")]                
                static internal extern Status GdipMultiplyWorldTransform (IntPtr graphics, IntPtr matrix, MatrixOrder order);
                
		[DllImport("gdiplus.dll")]
		static internal extern Status GdipRotateWorldTransform(IntPtr graphics, float angle, MatrixOrder order);
		[DllImport("gdiplus.dll")]
		static internal extern Status GdipTranslateWorldTransform(IntPtr graphics, float dx, float dy, MatrixOrder order);
		[DllImport("gdiplus.dll")]
                static internal extern Status GdipDrawBezier (IntPtr graphics, IntPtr pen, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
		[DllImport("gdiplus.dll")]
                static internal extern Status GdipDrawBezierI (IntPtr graphics, IntPtr pen, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
		[DllImport("gdiplus.dll")]
                static internal extern Status GdipDrawEllipseI (IntPtr graphics, IntPtr pen, int x, int y, int width, int height);
		[DllImport("gdiplus.dll")]
                static internal extern Status GdipDrawEllipse (IntPtr graphics, IntPtr pen, float x, float y, float width, float height);
		[DllImport("gdiplus.dll")]
		static internal extern Status GdipDrawLine (IntPtr graphics, IntPtr pen, float x1, float y1, float x2, float y2);
		[DllImport("gdiplus.dll")]
		static internal extern Status GdipDrawLineI (IntPtr graphics, IntPtr pen, int x1, int y1, int x2, int y2);
                [DllImport ("gdiplus.dll")]
                static internal extern Status GdipDrawLines (IntPtr graphics, IntPtr pen, PointF [] points, int count);
                [DllImport ("gdiplus.dll")]
                static internal extern Status GdipDrawLinesI (IntPtr graphics, IntPtr pen, Point [] points, int count);
                [DllImport ("gdiplus.dll")]
                static internal extern Status GdipDrawPie (IntPtr graphics, IntPtr pen, float x, float y, float width, float height, float startAngle, float sweepAngle);
                [DllImport ("gdiplus.dll")]
                static internal extern Status GdipDrawPieI (IntPtr graphics, IntPtr pen, int x, int y, int width, int height, float startAngle, float sweepAngle);
                [DllImport ("gdiplus.dll")]
                static internal extern Status GdipDrawPolygon (IntPtr graphics, IntPtr pen, PointF [] points, int count);
                [DllImport ("gdiplus.dll")]
                static internal extern Status GdipDrawPolygonI (IntPtr graphics, IntPtr pen, Point [] points, int count);
                [DllImport ("gdiplus.dll")]
                static internal extern Status GdipDrawRectangle (IntPtr graphics, IntPtr pen, float x, float y, float width, float height);
                [DllImport ("gdiplus.dll")]
                static internal extern Status GdipDrawRectangleI (IntPtr graphics, IntPtr pen, int x, int y, int width, int height);
		[DllImport("gdiplus.dll")]
                static internal extern Status GdipFillEllipseI (IntPtr graphics, IntPtr pen, int x, int y, int width, int height);
		[DllImport("gdiplus.dll")]
                static internal extern Status GdipFillEllipse (IntPtr graphics, IntPtr pen, float x, float y, float width, float height);
                [DllImport ("gdiplus.dll")]
                static internal extern  Status GdipFillPolygon (IntPtr graphics, IntPtr brush, PointF [] points, int count, FillMode fillMode);
                [DllImport ("gdiplus.dll")]
                static internal extern  Status GdipFillPolygonI (IntPtr graphics, IntPtr brush, Point [] points, int count, FillMode fillMode);
                [DllImport ("gdiplus.dll")]
                static internal extern  Status GdipFillPolygon2 (IntPtr graphics, IntPtr brush, PointF [] points, int count);
                [DllImport ("gdiplus.dll")]
                static internal extern  Status GdipFillPolygon2I (IntPtr graphics, IntPtr brush, Point [] points, int count);
                [DllImport("gdiplus.dll")]
		static internal extern Status GdipFillRectangle (IntPtr graphics, IntPtr brush, float x1, float y1, float x2, float y2);
		[DllImport("gdiplus.dll")]
		static internal extern Status GdipDrawString (IntPtr graphics, string text, int len, IntPtr font, ref GpRectF rc, IntPtr format, IntPtr brush);
		[DllImport("gdiplus.dll")]
		static internal extern Status GdipGetDC (IntPtr graphics, out int hdc);
		[DllImport("gdiplus.dll")]
		static internal extern Status GdipReleaseDC (IntPtr graphics, IntPtr hdc);
		[DllImport("gdiplus.dll")]
		static internal extern Status GdipDrawImageRectI (IntPtr graphics, IntPtr image, int x, int y, int width, int height);
		[DllImport ("gdiplus.dll")]
		static internal extern Status GdipGetRenderingOrigin (IntPtr graphics, out int x, out int y);
		[DllImport ("gdiplus.dll")]
		static internal extern Status GdipSetRenderingOrigin (IntPtr graphics, int x, int y);
 		[DllImport("gdiplus.dll")]
 		internal static extern Status GdipCloneBitmapAreaI (int x, int y, int width, int height, PixelFormat format, IntPtr original, out int bitmap);

                
		
		// Pen functions
		[DllImport("gdiplus.dll")]
		internal static extern Status GdipCreatePen1(int argb, float width, Unit unit, out int pen);
		[DllImport("gdiplus.dll")]
		internal static extern Status GdipDeletePen(IntPtr pen);
		[DllImport("gdiplus.dll")]
                internal static extern Status GdipSetPenMiterLimit (IntPtr pen, float miterLimit);
		[DllImport("gdiplus.dll")]
                internal static extern Status GdipGetPenMiterLimit (IntPtr pen, out float miterLimit);
		[DllImport("gdiplus.dll")]
                internal static extern Status GdipSetPenLineJoin (IntPtr pen, LineJoin lineJoin);
		[DllImport("gdiplus.dll")]
                internal static extern Status GdipGetPenLineJoin (IntPtr pen, out LineJoin lineJoin);
		[DllImport("gdiplus.dll")]
                internal static extern Status GdipSetPenLineCap (IntPtr pen, LineCap lineCap);
		[DllImport("gdiplus.dll")]
                internal static extern Status GdipGetPenLineCap (IntPtr pen, out LineCap lineCap);
		[DllImport("gdiplus.dll")]
                internal static extern Status GdipSetPenTransform (IntPtr pen, Matrix matrix);
		[DllImport("gdiplus.dll")]
                internal static extern Status GdipGetPenTransform (IntPtr pen, out Matrix matrix);
		
		// Bitmap functions
		[DllImport("gdiplus.dll")]
		internal static extern Status GdipCreateBitmapFromScan0 (int width, int height, int stride, PixelFormat format, IntPtr scan0, out int bitmap);

		[DllImport("gdiplus.dll")]
		internal static extern Status GdipCreateBitmapFromGraphics (int width, int height, IntPtr target, out int bitmap);

		[DllImport("gdiplus.dll")]
		internal static extern Status GdipBitmapLockBits (IntPtr bmp, ref GpRect rc, ImageLockMode flags, PixelFormat format, [In, Out] BitmapData bmpData);
		[DllImport("gdiplus.dll")]
		internal static extern Status ____BitmapLockBits (IntPtr bmp, ref GpRect  rc, ImageLockMode flags, PixelFormat format, ref int width, ref int height, ref int stride, ref int format2, ref int reserved, ref IntPtr scan0);
		
		[DllImport("gdiplus.dll")]
		internal static extern Status GdipBitmapUnlockBits (IntPtr bmp, [In,Out] BitmapData bmpData);

		// Image functions
		[DllImport("gdiplus.dll")]
		internal static extern Status GdipDisposeImage (IntPtr image);
		[DllImport("gdiplus.dll")]
		internal static extern Status GdipDrawImageI( IntPtr graphics, IntPtr image, int x, int y);
		[DllImport("gdiplus.dll")]
		internal static extern Status GdipGetImageGraphicsContext( IntPtr image, out int graphics);

                // Matrix functions
                [DllImport ("gdiplus.dll")]
                internal static extern Status GdipCreateMatrix (out IntPtr matrix);
                [DllImport ("gdiplus.dll")]
                internal static extern Status GdipCreateMatrix2 (float m11, float m12, float m21, float m22, float dx, float dy, out IntPtr matrix);
                [DllImport ("gdiplus.dll")]
                internal static extern Status GdipCreateMatrix3 (GpRectF rect, PointF[] dstplg, out IntPtr matrix);
                [DllImport ("gdiplus.dll")]                
                internal static extern Status GdipCreateMatrix3I (GpRect rect, Point[] dstplg, out IntPtr matrix);
                [DllImport ("gdiplus.dll")]
                internal static extern Status GdipDeleteMatrix (IntPtr matrix);

                [DllImport ("gdiplus.dll")]
                internal static extern Status GdipCloneMatrix (IntPtr matrix, out IntPtr cloneMatrix);
                [DllImport ("gdiplus.dll")]
                internal static extern Status GdipSetMatrixElements (IntPtr matrix, float m11, float m12, float m21, float m22, float dx, float dy);
                [DllImport ("gdiplus.dll")]
                internal static extern Status GdipGetMatrixElements (IntPtr matrix, out float[] matrixOut);
                [DllImport ("gdiplus.dll")]
                internal static extern Status GdipMultiplyMatrix (IntPtr matrix, IntPtr matrix2, MatrixOrder order);
                [DllImport ("gdiplus.dll")]
                internal static extern Status GdipTranslateMatrix (IntPtr matrix, float offsetX, float offsetY, MatrixOrder order);
                [DllImport ("gdiplus.dll")]
                internal static extern Status GdipScaleMatrix (IntPtr matrix, float scaleX, float scaleY, MatrixOrder order);
                [DllImport ("gdiplus.dll")]
                internal static extern Status GdipRotateMatrix (IntPtr matrix, float angle, MatrixOrder order);

                [DllImport ("gdiplus.dll")]
                internal static extern Status GdipShearMatrix (IntPtr matrix, float shearX, float shearY, MatrixOrder order);

                [DllImport ("gdiplus.dll")]
                internal static extern Status GdipInvertMatrix (IntPtr matrix);
                [DllImport ("gdiplus.dll")]
                internal static extern Status GdipTransformMatrixPoints (IntPtr matrix, PointF[] pts, int count);
                [DllImport ("gdiplus.dll")]
                internal static extern Status GdipTransformMatrixPointsI (IntPtr matrix, Point[] pts, int count);                
                [DllImport ("gdiplus.dll")]
                internal static extern Status GdipVectorTransformMatrixPoints (IntPtr matrix, PointF[] pts, int count);
                [DllImport ("gdiplus.dll")]
                internal static extern Status GdipVectorTransformMatrixPointsI (IntPtr matrix, Point[] pts, int count);
                [DllImport ("gdiplus.dll")]
                internal static extern Status GdipIsMatrixInvertible (IntPtr matrix, out bool result);

                [DllImport ("gdiplus.dll")]
                internal static extern Status GdipIsMatrixIdentity (IntPtr matrix, out bool result);
                [DllImport ("gdiplus.dll")]                
                internal static extern Status GdipIsMatrixEqual (IntPtr matrix, IntPtr matrix2, out bool result); 
		
		#endregion
	}
}
