#include "GeometricTransformer.h"

int main(int argc, char* argv[]) {
	string option, input, output, interp;
	GeometricTransformer* geometric = new GeometricTransformer();
	PixelInterpolate* pixelInterpolate;

	if (argc == 7) {
		option = argv[1];
		interp = argv[2];
		input = argv[5];
		output = argv[6];
		float x = stof(argv[3]);
		float y = stof(argv[4]);
		Mat srcImage = imread(input);
		Mat dstImage;
		if (srcImage.empty())
			return 0;
		if (option == "-zoom") {
			if (interp == "-bl") {
				pixelInterpolate = new BilinearInterpolate();
				geometric->Scale(srcImage, dstImage, x, y, pixelInterpolate);
				imwrite(output, dstImage);
			}
			if (interp == "-nn") {
				pixelInterpolate = new NearestNeighborInterpolate();
				geometric->Scale(srcImage, dstImage, x, y, pixelInterpolate);
				imwrite(output, dstImage);
			}
		}
		if (option == "-resize") {
			if (interp == "-bl") {
				pixelInterpolate = new BilinearInterpolate();
				geometric->Resize(srcImage, dstImage, x, y, pixelInterpolate);
				imwrite(output, dstImage);
			}
			if (interp == "-nn") {
				pixelInterpolate = new NearestNeighborInterpolate();
				geometric->Resize(srcImage, dstImage, x, y, pixelInterpolate);
				imwrite(output, dstImage);
			}
		}
		cout << "Successfully!" << endl;
	}
	if (argc == 6) {
		option = argv[1];
		interp = argv[2];
		input = argv[4];
		output = argv[5];
		float angle = stof(argv[3]);
		Mat srcImage = imread(input);
		Mat dstImage;
		if (srcImage.empty())
			return 0;
		if (option == "-rotK") {
			if (interp == "-bl") {
				pixelInterpolate = new BilinearInterpolate();
				geometric->RotateKeepImage(srcImage, dstImage, angle, pixelInterpolate);
				imwrite(output, dstImage);
			}
			if (interp == "-nn") {
				pixelInterpolate = new NearestNeighborInterpolate();
				geometric->RotateKeepImage(srcImage, dstImage, angle, pixelInterpolate);
				imwrite(output, dstImage);
			}
		}
		if (option == "-rotP") {
			if (interp == "-bl") {
				pixelInterpolate = new BilinearInterpolate();
				geometric->RotateUnkeepImage(srcImage, dstImage, angle, pixelInterpolate);
				imwrite(output, dstImage);
			}
			if (interp == "-nn") {
				pixelInterpolate = new NearestNeighborInterpolate();
				geometric->RotateUnkeepImage(srcImage, dstImage, angle, pixelInterpolate);
				imwrite(output, dstImage);
			}
		}
		cout << "Successfully!" << endl;
	}
	if (argc == 5) {
		option = argv[1];
		interp = argv[2];
		input = argv[3];
		output = argv[4];
		Mat srcImage = imread(input);
		Mat dstImage;
		if (srcImage.empty())
			return 0;
		if (option == "-flipV") {
			if (interp == "-bl") {
				pixelInterpolate = new BilinearInterpolate();
				geometric->Flip(srcImage, dstImage, 0, pixelInterpolate);
				imwrite(output, dstImage);
			}
			if (interp == "-nn") {
				pixelInterpolate = new NearestNeighborInterpolate();
				geometric->Flip(srcImage, dstImage, 0, pixelInterpolate);
				imwrite(output, dstImage);
			}
		}
		if (option == "-flipH") {
			if (interp == "-bl") {
				pixelInterpolate = new BilinearInterpolate();
				geometric->Flip(srcImage, dstImage, 1, pixelInterpolate);
				imwrite(output, dstImage);
			}
			if (interp == "-nn") {
				pixelInterpolate = new NearestNeighborInterpolate();
				geometric->Flip(srcImage, dstImage, 1, pixelInterpolate);
				imwrite(output, dstImage);
			}
		}
		cout << "Successfully!" << endl;
	}
} 