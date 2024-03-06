#include "GeometricTransformer.h"

PixelInterpolate::PixelInterpolate() {}
PixelInterpolate::~PixelInterpolate() {}

void BilinearInterpolate::Interpolate(float tx, float ty, uchar* pSrc, uchar* pDst, int srcWidthStep, int nChannels)
{
	int l = round(tx);
	int r = round(ty);
	float a = tx - l;
	float b = ty - r;
	for (int i = 0; i < nChannels; i++) {
		pDst[i] = (uchar)((1 - a) * (1 - b) * ((int)pSrc[l * srcWidthStep + r * nChannels + i])
			+ a * (1 - b) * ((int)pSrc[(l + 1) * srcWidthStep + r * nChannels + i])
			+ b * (1 - a) * ((int)pSrc[l * srcWidthStep + (r + 1) * nChannels + i])
			+ a * b * ((int)pSrc[(l + 1) * srcWidthStep + (r + 1) * nChannels + i]));
	}
}
BilinearInterpolate::BilinearInterpolate() {}
BilinearInterpolate::~BilinearInterpolate() {}

void NearestNeighborInterpolate::Interpolate(float tx, float ty, uchar* pSrc, uchar* pDst, int srcWidthStep, int nChannels)
{
	int x = round(tx);
	int y = round(ty);
	for (int i = 0; i < nChannels; i++) {
		pDst[i] = (uchar)(pSrc[x * srcWidthStep + y * nChannels + i]);
	}
}
NearestNeighborInterpolate::NearestNeighborInterpolate() {}
NearestNeighborInterpolate::~NearestNeighborInterpolate() {}

void AffineTransform::Translate(float dx, float dy)
{
	Mat translate = (Mat_<float>(3, 3) << 1, 0, 0, 0, 1, 0, dx, dy, 1);
	_matrixTransform = _matrixTransform * translate;
}
void AffineTransform::Rotate(float angle)
{
	angle = angle * 3.1415926 / 180;
	Mat rotate = (Mat_<float>(3, 3) << cos(angle), sin(angle), 0, -sin(angle), cos(angle), 0, 0, 0, 1);
	_matrixTransform = _matrixTransform * rotate;
}
void AffineTransform::Scale(float sx, float sy)
{
	Mat scale = (Mat_<float>(3, 3) << sx, 0, 0, 0, sy, 0, 0, 0, 1);
	_matrixTransform = _matrixTransform * scale;
}
void AffineTransform::TransformPoint(float& x, float& y)
{
	// Tạo 2 ma trận điểm ảnh nguồn và điểm ảnh đích
	Mat original = (Mat_<float>(1, 3) << x, y, 1);
	Mat result = original * _matrixTransform;
	// Gán lại giá trị mới cho điểm ảnh 
	x = result.at<float>(0, 0);
	y = result.at<float>(0, 1);
}
void AffineTransform::Inverse()
{
	_matrixTransform = _matrixTransform.inv();
}
void AffineTransform::Reflection(int opt) {
	if (opt == 1) {
		Mat reflect = (Mat_<float>(3, 3) << -1, 0, 0, 0, 1, 0, 0, 0, 1);
		_matrixTransform = _matrixTransform * reflect;
	}
	if (opt == 0) {
		Mat reflect = (Mat_<float>(3, 3) << 1, 0, 0, 0, -1, 0, 0, 0, 1);
		_matrixTransform = _matrixTransform * reflect;
	}
}
AffineTransform::AffineTransform()
{
	_matrixTransform = (Mat_<float>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1);
}
AffineTransform::~AffineTransform() {}

int GeometricTransformer::Transform(const Mat& beforeImage, Mat& afterImage, AffineTransform* transformer, PixelInterpolate* interpolator)
{
	if (beforeImage.data == NULL)
		return 0;
	// Chiều rông và chiều cao của ảnh gốc và ảnh đích
	int srcWidth = beforeImage.cols, srcHeight = beforeImage.rows;
	int dstWidth = afterImage.cols, dstHeight = afterImage.rows;
	// Số kênh màu của ảnh gốc
	int nChannels = beforeImage.channels();
	// WidthStep của ảnh gốc và ảnh đích
	int srcWidthStep = beforeImage.step[0];
	int dstWidthStep = afterImage.step[0];

	uchar* pSrcData = (uchar*)beforeImage.data;
	uchar* pDstData = (uchar*)afterImage.data;

	// Chuyển ma trận của các phép biến đổi AffineTransform thành ma trận nghịch đảo
	transformer->Inverse();
	for (int y = 0; y < dstHeight; y++, pDstData += dstWidthStep) {
		uchar* pRow = pDstData;
		for (int x = 0; x < dstWidth; x++, pRow += nChannels) {
			float yTemp = y, xTemp = x;
			transformer->TransformPoint(yTemp, xTemp);
			if (round(xTemp) < srcWidth && round(yTemp) < srcHeight && xTemp >= 0 && yTemp >= 0)
				interpolator->Interpolate(yTemp, xTemp, pSrcData, pRow, srcWidthStep, nChannels);
		}
	}
	return 1;
}

int GeometricTransformer::RotateKeepImage(const Mat& srcImage, Mat& dstImage, float angle, PixelInterpolate* interpolator)
{
	if (srcImage.data == NULL)
		return 0;
	int srcWidth = srcImage.cols, srcHeight = srcImage.rows;
	float cosx = cos(angle * 3.1415926 / 180.0);
	float sinx = sin(angle * 3.1415926 / 180.0);
	// Tìm kích thước và tạo cửa số ảnh mới
	int dstWidth = (int)(srcWidth * cosx + srcHeight * sinx);
	int dstHeight = (int)(srcWidth * sinx + srcHeight * cosx);
	dstImage.create(dstHeight, dstWidth, srcImage.type());

	AffineTransform* affineTransform = new AffineTransform();
	
	// Điểu chỉnh ảnh về giữa khung hình mới
	affineTransform->Translate((dstHeight / 2) - (srcHeight / 2), (dstWidth / 2) - (srcWidth / 2));
	// Kéo tâm của ảnh về gốc tọa độ để thực hiện phép Rotate ở chính giữa ảnh
	affineTransform->Translate(-(dstHeight / 2), -(dstWidth / 2));
	affineTransform->Rotate(angle);
	// Trả ảnh về vị trí giữa khung hình mới
	affineTransform->Translate(dstHeight / 2, dstWidth / 2);

	GeometricTransformer::Transform(srcImage, dstImage, affineTransform, interpolator);

	imshow("Source image", srcImage);
	imshow("Rotate keep image", dstImage);
	waitKey(0);
	return 1;
}

int GeometricTransformer::RotateUnkeepImage(const Mat& srcImage, Mat& dstImage, float angle, PixelInterpolate* interpolator)
{
	if (srcImage.data == NULL)
		return 0;
	int srcWidth = srcImage.cols;
	int srcHeight = srcImage.rows;
	// Tạo cửa sổ ảnh mới cùng kích thước cùng loại với ảnh gốc
	dstImage.create(srcHeight, srcWidth, srcImage.type());

	AffineTransform* affineTransform = new AffineTransform();

	// Kéo tâm của ảnh về gốc tọa độ để thực hiện phép Rotate ở chính giữa ảnh
	affineTransform->Translate(-(srcHeight / 2), -(srcWidth / 2));
	affineTransform->Rotate(angle);
	// Trả ảnh về vị trí giữa khung hình mới
	affineTransform->Translate(srcHeight / 2, srcWidth / 2);

	GeometricTransformer::Transform(srcImage, dstImage, affineTransform, interpolator);

	imshow("Source image", srcImage);
	imshow("Rotate unkeep image", dstImage);
	waitKey(0);
	return 1;
}

int GeometricTransformer::Scale(const Mat& srcImage, Mat& dstImage, float sx, float sy, PixelInterpolate* interpolator)
{
	if (srcImage.data == NULL) 
		return 0;
	int srcWidth = srcImage.cols;
	int srcHeight = srcImage.rows;
	// Tạo cửa sổ ảnh mới với cùng kích thước và cùng loại với ảnh gốc
	dstImage.create(srcHeight, srcWidth, srcImage.type());

	AffineTransform* aff = new AffineTransform();

	// Kéo tâm của ảnh về gốc tọa độ để thực hiện phép phóng to/ thu nhỏ ở chính giữa ảnh
	aff->Translate(-srcHeight / 2, -srcWidth / 2);
	// Điều chỉnh kích thước 
	aff->Scale(sy, sx);
	// Trả ảnh về vị trí gốc tọa độ (vị trí khung hình mới)
	aff->Translate(srcHeight / 2, srcWidth / 2);

	GeometricTransformer::Transform(srcImage, dstImage, aff, interpolator);

	imshow("Source image", srcImage);
	imshow("Scale image", dstImage);
	waitKey(0);
	return 1;
}
int GeometricTransformer::Resize(const Mat& srcImage, Mat& dstImage, int newWidth, int newHeight, PixelInterpolate* interpolator) {
	if (srcImage.data == NULL)
		return 0;
	int srcWidth = srcImage.cols;
	int srcHeight = srcImage.rows;
	// Tính tỷ lệ ảnh mới
	float sx = (float)newWidth / (float)srcWidth;
	float sy = (float)newHeight / (float)srcHeight;
	// Tạo cửa sổ ảnh mới với kích thước mới và cùng loại với ảnh gốc
	dstImage.create(newHeight, newWidth, srcImage.type());

	AffineTransform* aff = new AffineTransform();

	// Kéo tâm của ảnh về gốc tọa độ để thực hiện phép phóng to/ thu nhỏ ở chính giữa ảnh
	aff->Translate(-srcHeight / 2, -srcWidth / 2);
	// Điều chỉnh kích thước 
	aff->Scale(sy, sx);
	// Trả ảnh về vị trí gốc tọa độ (vị trí khung hình mới)
	aff->Translate(newHeight / 2, newWidth / 2);

	GeometricTransformer::Transform(srcImage, dstImage, aff, interpolator);

	imshow("Source image", srcImage);
	imshow("Scale image", dstImage);
	waitKey(0);
	return 1;
}
int GeometricTransformer::Flip(const Mat& srcImage, Mat& dstImage, bool direction, PixelInterpolate* interpolator) {
	if (srcImage.data == NULL)
		return 0;
	int srcWidth = srcImage.cols;
	int srcHeight = srcImage.rows;
	// Tạo cửa sổ ảnh mới với kích thước mới và cùng loại với ảnh gốc
	dstImage.create(srcHeight, srcWidth, srcImage.type());

	AffineTransform* aff = new AffineTransform();
	if (direction == 1) {
		aff->Reflection(1);
		aff->Translate(srcHeight, 0);
	} 
	if (direction == 0) {
		aff->Reflection(0);
		aff->Translate(0, srcWidth);
	}

	GeometricTransformer::Transform(srcImage, dstImage, aff, interpolator);

	imshow("Source image", srcImage);
	imshow("Flip image", dstImage);
	waitKey(0);
	return 1;
}
GeometricTransformer::GeometricTransformer() {}
GeometricTransformer::~GeometricTransformer() {}