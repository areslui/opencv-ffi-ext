/*Index: src/detectors.cpp
===================================================================
--- src/detectors.cpp	(revisione 5053)
+++ src/detectors.cpp	(copia locale)
@@ -132,6 +132,14 @@
         pos += string("Dynamic").size();
         fd = new DynamicAdaptedFeatureDetector( AdjusterAdapter::create(detectorType.substr(pos)) );
     }
+    else if( !detectorType.compare( "HarrisLaplace" ) )
+    {
+        fd = new HarrisLaplaceFeatureDetector();
+    }
+    else if( !detectorType.compare( "HarrisAffine" ) )
+    {
+        fd = new HarrisAffineFeatureDetector();
+    }
 
     return fd;
 } */

#include "harris_laplace.hpp"
#include "gaussian_pyramid.hpp"
#include "cv_sift_wrapped.h"

//#define POW2(x) (0x1 << x)

namespace cv {

  /*
   *  HarrisLaplaceFeatureDetector
   */
  HarrisLaplaceFeatureDetector::Params::Params(int _numOctaves, float _quality_level, float _DOG_thresh, int _maxCorners, int _num_layers, float _harris_k ) :
    numOctaves(_numOctaves), quality_level(_quality_level), DOG_thresh(_DOG_thresh), maxCorners(_maxCorners), num_layers(_num_layers), harris_k( _harris_k )
  {}
  HarrisLaplaceFeatureDetector::HarrisLaplaceFeatureDetector( int numOctaves, float quality_level, float DOG_thresh, int maxCorners, int num_layers, float harris_k )
    : harris( numOctaves, quality_level, DOG_thresh, maxCorners, num_layers, harris_k )
  {}

  HarrisLaplaceFeatureDetector::HarrisLaplaceFeatureDetector(  const Params& params  )
    : harris( params.numOctaves, params.quality_level, params.DOG_thresh, params.maxCorners, params.num_layers, params.harris_k )

  {}

  void HarrisLaplaceFeatureDetector::read (const FileNode& fn)
  {
    int numOctaves = fn["numOctaves"];
    float quality_level = fn["quality_level"];
    float DOG_thresh = fn["DOG_thresh"];
    int maxCorners = fn["maxCorners"];
    int num_layers = fn["num_layers"];
    float harris_k = fn["harris_k"];

    harris = HarrisLaplace( numOctaves, quality_level, DOG_thresh, maxCorners,num_layers, harris_k );
  }

  void HarrisLaplaceFeatureDetector::write (FileStorage& fs) const
  {

    fs << "numOctaves" << harris.numOctaves;
    fs << "quality_level" << harris.quality_level;
    fs << "DOG_thresh" << harris.DOG_thresh;
    fs << "maxCorners" << harris.maxCorners;
    fs << "num_layers" << harris.num_layers;
    fs << "harris_k" << harris.harris_k;


  }


  void HarrisLaplaceFeatureDetector::detectImpl( const Mat& image, vector<KeyPoint>& keypoints, const Mat& mask ) const
  {

    harris.detect(image, keypoints);
  }

  /*
   *  HarrisAffineFeatureDetector
   */
  HarrisAffineFeatureDetector::Params::Params(int _numOctaves, float _quality_level, float _DOG_thresh, int _maxCorners, int _num_layers, float _harris_k ) :
    numOctaves(_numOctaves), quality_level(_quality_level), DOG_thresh(_DOG_thresh), maxCorners(_maxCorners), num_layers(_num_layers), harris_k( _harris_k )
  {}
  HarrisAffineFeatureDetector::HarrisAffineFeatureDetector( int numOctaves, float quality_level, float DOG_thresh, int maxCorners, int num_layers, float harris_k )
    : harris( numOctaves, quality_level, DOG_thresh, maxCorners, num_layers, harris_k )
  {}

  HarrisAffineFeatureDetector::HarrisAffineFeatureDetector(  const Params& params  )
    : harris( params.numOctaves, params.quality_level, params.DOG_thresh, params.maxCorners, params.num_layers, params.harris_k)

  {}

  void HarrisAffineFeatureDetector::read (const FileNode& fn)
  {
    int numOctaves = fn["numOctaves"];
    float quality_level = fn["quality_level"];
    float DOG_thresh = fn["DOG_thresh"];
    int maxCorners = fn["maxCorners"];
    int num_layers = fn["num_layers"];
    float harris_k = fn["harris_k"];

    harris = HarrisLaplace( numOctaves, quality_level, DOG_thresh, maxCorners,num_layers, harris_k );
  }

  void HarrisAffineFeatureDetector::write (FileStorage& fs) const
  {

    fs << "numOctaves" << harris.numOctaves;
    fs << "quality_level" << harris.quality_level;
    fs << "DOG_thresh" << harris.DOG_thresh;
    fs << "maxCorners" << harris.maxCorners;
    fs << "num_layers" << harris.num_layers;
    fs << "harris_k"   << harris.harris_k;


  }
  void HarrisAffineFeatureDetector::detect( const Mat& image, vector<Elliptic_KeyPoint>& ekeypoints, const Mat& mask ) const
  {
    vector<KeyPoint> keypoints;
    harris.detect(image, keypoints);
    Mat fimage;
    image.convertTo(fimage, CV_32F, 1.f/255);
    calcAffineCovariantRegions(fimage, keypoints, ekeypoints, "HarrisLaplace");
  }

  void HarrisAffineFeatureDetector::detectImpl( const Mat& image, vector<KeyPoint>& ekeypoints, const Mat& mask ) const
  {
  }


  bool sort_func(KeyPoint kp1, KeyPoint kp2);

  /**
   * Default constructor of HarrisLaplace
   */
  HarrisLaplace::HarrisLaplace()
  {
  }

  /**
   * Constructor of HarrisLaplace
   * _numOctaves: number of octaves in the gaussian pyramid
   * _quality_level: cornerness threshold. The value of the parameter is multiplied by the higher cornerness value. The corners, which cornerness is lower than the product, will be rejected.
   * _DOG_thresh: DoG threshold. Corners that have DoG response lower than _DOG_thresh will be rejected.
   * _maxCorners: Maximum number of keypoints to return. Keypoints returned are the strongest.
   * _num_layers: number of layers in the gaussian pyramid. Accepted value are 2 or 4 so smoothing step between layer will be 1.4 or 1.2
   */
  HarrisLaplace::HarrisLaplace(int _numOctaves, float _quality_level, float _DOG_thresh, int _maxCorners,
      int _num_layers, float _harris_k ) :
    numOctaves(_numOctaves), quality_level(_quality_level), DOG_thresh(_DOG_thresh),
    maxCorners(_maxCorners), num_layers(_num_layers), harris_k(_harris_k)
  {
    assert(num_layers == 2 || num_layers==4);
  }

  /**
   * Destructor
   */
  HarrisLaplace::~HarrisLaplace()
  {
  }

  /**
   * Detect method
   * The method detect Harris corners on scale space as described in
   * "K. Mikolajczyk and C. Schmid.
   * Scale & affine invariant interest point detectors.
   * International Journal of Computer Vision, 2004"
   */
  void HarrisLaplace::detect(const Mat & image, vector<KeyPoint>& keypoints) const
  {
    Mat_<float> dx2, dy2, dxy;
    Mat Lx, Ly;
    float si, sd;
    int gsize;
    Mat fimage;
    image.convertTo(fimage, CV_32F, 1.f/255);
    /*Build gaussian pyramid*/
    Pyramid pyr(fimage, numOctaves, num_layers, 1, -1, true);
    keypoints = vector<KeyPoint> (0);

    /*Find Harris corners on each layer*/
    for (int octave = 0; octave <= numOctaves; octave++)
    {
      for (int layer = 1; layer <= num_layers; layer++)
      {
        if (octave == 0)
          layer = num_layers;

        Mat Lxm2smooth, Lxmysmooth, Lym2smooth;

        si = pow(2, layer / (float) num_layers);
        sd = si * 0.7;

        Mat curr_layer;
        if (num_layers == 4)
        {
          if (layer == 1)
          {
            Mat tmp = pyr.getLayer(octave - 1, num_layers - 1);
            resize(tmp, curr_layer, Size(0, 0), 0.5, 0.5, INTER_AREA);

          } else
            curr_layer = pyr.getLayer(octave, layer - 2);
        } else /*if num_layer==2*/
        {

          curr_layer = pyr.getLayer(octave, layer - 1);
        }

        /*Calculates second moment matrix*/

        /*Derivatives*/
        Sobel(curr_layer, Lx, CV_32F, 1, 0, 1);
        Sobel(curr_layer, Ly, CV_32F, 0, 1, 1);

        /*Normalization*/
        Lx = Lx * sd;
        Ly = Ly * sd;

        Mat Lxm2 = Lx.mul(Lx);
        Mat Lym2 = Ly.mul(Ly);
        Mat Lxmy = Lx.mul(Ly);

        gsize = (int)(ceil(si * 3.f) * 2 + 1);

        /*Convolution*/
        GaussianBlur(Lxm2, Lxm2smooth, Size(gsize, gsize), si, si, BORDER_REPLICATE);
        GaussianBlur(Lym2, Lym2smooth, Size(gsize, gsize), si, si, BORDER_REPLICATE);
        GaussianBlur(Lxmy, Lxmysmooth, Size(gsize, gsize), si, si, BORDER_REPLICATE);

        Mat cornern_mat(curr_layer.size(), CV_32F);

        /*Calculates cornerness in each pixel of the image*/
        for (int row = 0; row < curr_layer.rows; row++)
        {
          for (int col = 0; col < curr_layer.cols; col++)
          {
            float dx2f = Lxm2smooth.at<float> (row, col);
            float dy2f = Lym2smooth.at<float> (row, col);
            float dxyf = Lxmysmooth.at<float> (row, col);
            float det = dx2f * dy2f - dxyf * dxyf;
            float tr = dx2f + dy2f;
            float cornerness = det - (harris_k * tr * tr);
            cornern_mat.at<float> (row, col) = cornerness;
          }
        }

        double maxVal = 0;
        Mat corn_dilate;

        /*Find max cornerness value and rejects all corners that are lower than a threshold*/
        minMaxLoc(cornern_mat, 0, &maxVal, 0, 0);
        threshold(cornern_mat, cornern_mat, maxVal * quality_level, 0, THRESH_TOZERO);
        dilate(cornern_mat, corn_dilate, Mat());

        Size imgsize = curr_layer.size();

        /*Verify for each of the initial points whether the DoG attains a maximum at the scale of the point*/
        Mat prevDOG, curDOG, succDOG;
        prevDOG = pyr.getDOGLayer(octave, layer - 1);
        curDOG = pyr.getDOGLayer(octave, layer);
        succDOG = pyr.getDOGLayer(octave, layer + 1);

        for (int y = 1; y < imgsize.height - 1; y++)
        {
          for (int x = 1; x < imgsize.width - 1; x++)
          {
            float val = cornern_mat.at<float> (y, x);
            if (val != 0 && val == corn_dilate.at<float> (y, x))
            {

              float curVal = curDOG.at<float> (y, x);
              float prevVal =  prevDOG.at<float> (y, x);
              float succVal = succDOG.at<float> (y, x);

              KeyPoint kp(
                  Point(x * pow((float)2, octave - 1) + pow((float)2, octave - 1) / 2,
                    y * pow((float)2, octave - 1) + pow((float)2, octave - 1) / 2),
                  3 * pow((float)2, octave - 1) * si * 2, 0, val, octave);

              /*Check whether keypoint size is inside the image*/
              float start_kp_x = kp.pt.x - kp.size / 2;
              float start_kp_y = kp.pt.y - kp.size / 2;
              float end_kp_x = start_kp_x + kp.size;
              float end_kp_y = start_kp_y + kp.size;

              if (curVal > prevVal && curVal > succVal && curVal >= DOG_thresh
                  && start_kp_x > 0 && start_kp_y > 0 && end_kp_x < image.cols
                  && end_kp_y < image.rows)
                keypoints.push_back(kp);

            }
          }
        }

      }

    }

    /*Sort keypoints in decreasing cornerness order*/
    sort(keypoints.begin(), keypoints.end(), sort_func);
    for (size_t i = 1; i < keypoints.size(); i++)
    {
      float max_diff = pow((float)2.0, (float)keypoints[i].octave + 1.f / 2);


      if (keypoints[i].response == keypoints[i - 1].response && norm(
            keypoints[i].pt - keypoints[i - 1].pt) <= max_diff)
      {

        float x = (keypoints[i].pt.x + keypoints[i - 1].pt.x) / 2;
        float y = (keypoints[i].pt.y + keypoints[i - 1].pt.y) / 2;

        keypoints[i].pt = Point(x, y);
        --i;
        keypoints.erase(keypoints.begin() + i);

      }
    }

    /*Select strongest keypoints*/
    if (maxCorners > 0 && maxCorners < (int) keypoints.size())
      keypoints.resize(maxCorners);


  }

  bool sort_func(KeyPoint kp1, KeyPoint kp2)
  {
    return (kp1.response > kp2.response);
  }


  extern "C" {

    CvSeq *cvHarrisLaplaceDetector( const CvArr *image, CvMemStorage *storage, CvHarrisLaplaceParams params )

    {
      vector<KeyPoint> kps;
      CvMat stub;
      Mat imgMat( cvGetMat( image, &stub ) );
      //Mat maskMat;
      //if( mask )
      //  maskMat = cvGetMat( mask, &stub );

      // HarrisLaplace exists as a standalone class, as well as a FeatureDetector
      // use the standalone ... 
      HarrisLaplaceFeatureDetector harrislaplace( params.numOctaves, params.quality_level, params.DOG_thresh, params.maxCorners, params.num_layers, params.harris_k );
      harrislaplace.detect( imgMat, kps );

      return KeyPointsToCvSeq( kps, storage );
    }

    static CvEllipticKeyPoint_t EllipticKeyPointToEllipticKeyPoint_t( const Elliptic_KeyPoint &kp )
    {
      CvEllipticKeyPoint_t keypoint;

      keypoint.centre.x = kp.centre.x;
      keypoint.centre.y = kp.centre.y;
      keypoint.axes.width = kp.axes.width;
      keypoint.axes.height = kp.axes.height;
      keypoint.phi = kp.phi;
      keypoint.size = kp.size;
      keypoint.si = kp.si;
      keypoint.transf = kp.transf;

      return keypoint;
    }

    static CvSeq *EllipticKeyPointsToCvSeq( vector<Elliptic_KeyPoint> kps, CvMemStorage *storage )
    {
      CvSeq *seq = cvCreateSeq( 0, sizeof( CvSeq ), sizeof( CvEllipticKeyPoint_t ), storage );

      CvSeqWriter writer;
      cvStartAppendToSeq( seq, &writer );
      for( vector<Elliptic_KeyPoint>::iterator itr = kps.begin(); itr != kps.end(); itr++ ) {
        CvEllipticKeyPoint_t kp = EllipticKeyPointToEllipticKeyPoint_t( *itr );
        CV_WRITE_SEQ_ELEM( kp, writer );
      }
      cvEndWriteSeq( &writer );

      //printf("After conversion, vector size = %d, CvSeq size = %d\n", kps.size(), seq->total );

      assert( kps.size() == seq->total );

      return seq;
    }


    CvSeq *cvHarrisAffineDetector( const CvArr *image, CvMemStorage *storage, CvHarrisLaplaceParams params ){
      vector<Elliptic_KeyPoint> kps;
      CvMat stub;
      Mat imgMat( cvGetMat( image, &stub ) );
      //Mat maskMat;
      //if( mask )
      //  maskMat = cvGetMat( mask, &stub );

      HarrisAffineFeatureDetector harrisaffine( params.numOctaves, params.quality_level, params.DOG_thresh, params.maxCorners, params.num_layers, params.harris_k );
      harrisaffine.detect( imgMat, kps );

      return EllipticKeyPointsToCvSeq( kps, storage ); 
    }


  }

} // namespace cv
