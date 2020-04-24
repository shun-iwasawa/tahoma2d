#pragma once

#ifndef STOPMOTION_H
#define STOPMOTION_H

#ifdef WITH_CANON
// Canon Includes
#include "EDSDK.h"
#include "EDSDKErrors.h"
#include "EDSDKTypes.h"
#endif

#include "turbojpeg.h"
#include "opencv2/opencv.hpp"

// Toonz Includes
#include "traster.h"
#include "toonzqt/gutil.h"
#include "toonzqt/dvdialog.h"

#include <QObject>

class QCamera;
class QCameraInfo;
class QDialog;
class QTimer;
class QSerialPort;

enum ASPECT_RATIO { FOUR_THREE = 0, THREE_TWO, SIXTEEN_NINE, OTHER_RATIO };

#include <QThread>
//=============================================================================
// JpgConverter
//-----------------------------------------------------------------------------

class JpgConverter : public QThread {
  Q_OBJECT
#ifdef WITH_CANON
  EdsStreamRef m_stream;
#endif
  TRaster32P m_finalImage;
  bool m_scale     = false;
  int m_scaleWidth = 0;

public:
  JpgConverter();
  ~JpgConverter();
#ifdef WITH_CANON
  void setStream(EdsStreamRef stream);
  void setScale(bool scale) { m_scale = scale; }
  void setScaleWidth(bool scaleWidth) { m_scaleWidth = scaleWidth; }
  TRaster32P getImage() { return m_finalImage; }
  void convertFromJpg();

protected:
  void run() override;
#endif

signals:
  void imageReady(bool);
};

//#endif

class StopMotion : public QObject {  // Singleton
  Q_OBJECT

public:
  static StopMotion* instance() {
    static StopMotion _instance;
    return &_instance;
  };

private:
  StopMotion();
  ~StopMotion();

  int m_opacity              = 255.0;
  int m_xSheetFrameNumber    = 1;
  int m_frameNumber          = 1;
  int m_subsampling          = 1;
  QString m_levelName        = "";
  QString m_fileType         = "jpg";
  QString m_filePath         = "+extras";
  QString m_frameInfoText    = "";
  QString m_infoColorName    = "";
  QString m_frameInfoToolTip = "";
  QSize m_allowedCameraSize;
  QStringList m_isoOptions, m_shutterSpeedOptions, m_apertureOptions,
      m_exposureOptions, m_whiteBalanceOptions, m_colorTempOptions,
      m_imageQualityOptions, m_pictureStyleOptions, m_controlBaudRates,
      m_serialDevices;

  QDialog *m_fullScreen1, *m_fullScreen2, *m_fullScreen3;
  int m_screenCount;
  bool m_useMjpg                 = true;
  bool m_useNumpadShortcuts      = false;
  bool m_numpadForStyleSwitching = true;
  bool m_turnOnRewind            = false;

  QTimer* m_reviewTimer;
  
  std::map<std::string, QAction*> m_oldActionMap;

  // Webcam Properties
  QList<QCameraInfo> m_webcams;
  QCamera* m_webcam;
  cv::VideoCapture m_cvWebcam;

  QSerialPort* m_serialPort;

#ifdef WITH_CANON
  std::map<EdsUInt32, std::string> m_avMap, m_tvMap, m_isoMap, m_modeMap,
      m_exposureMap, m_whiteBalanceMap, m_imageQualityMap, m_pictureStyleMap;
  JpgConverter* m_converter;
  static EdsError EDSCALLBACK handleObjectEvent(EdsObjectEvent event,
                                                EdsBaseRef object,
                                                EdsVoid* context);

  static EdsError EDSCALLBACK handlePropertyEvent(EdsPropertyEvent event,
                                                  EdsPropertyID property,
                                                  EdsUInt32 param,
                                                  EdsVoid* context);

  static EdsError EDSCALLBACK handleStateEvent(EdsStateEvent event,
                                               EdsUInt32 parameter,
                                               EdsVoid* context);
  static EdsError EDSCALLBACK handleCameraAddedEvent(EdsVoid* context);

  void buildAvMap();
  void buildTvMap();
  void buildIsoMap();
  void buildModeMap();
  void buildExposureMap();
  void buildWhiteBalanceMap();
  void buildImageQualityMap();
  void buildPictureStyleMap();

#endif

public:
  enum LiveViewStatus {
    LiveViewClosed = 0,
    LiveViewStarting,
    LiveViewOpen,
    LiveViewPaused
  };

  bool m_useScaledImages = true;
  bool m_usingWebcam     = false;
  bool m_blackCapture    = true;
  bool m_placeOnXSheet   = true;
  int m_reviewTime       = 2;
  int m_liveViewStatus   = LiveViewClosed;
  bool m_hasLiveViewImage, m_hasLineUpImage, m_showLineUpImage;
  bool m_alwaysLiveView     = false;
  bool m_active             = false;
  bool m_userCalledPause    = false;
  bool m_converterSucceeded = false;
  bool m_drawBeneathLevels  = true;
  bool m_isTimeLapse        = false;
  QString m_tempFile;
  QTimer* m_timer;
  QList<QSize> m_webcamResolutions;
  int m_intervalTime = 10;
  bool m_intervalStarted = false;
  QTimer* m_intervalTimer, * m_countdownTimer;

  // Canon Public Properties
  bool m_pickLiveViewZoom = false;
#ifdef WITH_CANON
  EdsError m_error              = EDS_ERR_OK;
  EdsUInt32 m_count             = 0;
  EdsCameraListRef m_cameraList = NULL;
  EdsCameraRef m_camera         = NULL;
  EdsUInt32 m_liveViewZoom      = 1;
  bool m_isSDKLoaded            = false;
  bool m_sessionOpen            = false;
  bool m_zooming                = false;
  std::string m_cameraName;
  TDimension m_proxyImageDimensions = TDimension(0, 0);
  TPointD m_proxyDpi                = TPointD(0.0, 0.0);
  TPoint m_liveViewZoomOffset       = TPoint(0, 0);
  bool m_liveViewZoomReadyToPick    = true;
  TPointD m_liveViewZoomPickPoint   = TPointD(0.0, 0.0);
  TPoint m_zoomRectDimensions       = TPoint(0, 0);
  TPoint m_calculatedZoomPoint      = TPoint(0, 0);
  TPoint m_finalZoomPoint           = TPoint(0, 0);
  TRect m_zoomRect                  = TRect(0, 0, 0, 0);
#endif

  TRaster32P m_liveViewImage, m_newImage, m_lineUpImage;
  TDimension m_liveViewImageDimensions = TDimension(0, 0);
  TDimension m_fullImageDimensions     = TDimension(0, 0);
  TPointD m_liveViewDpi                = TPointD(0.0, 0.0);
  TPointD m_fullImageDpi               = TPointD(0.0, 0.0);
  // Webcam Public Properties
  QString m_webcamDeviceName;
  QString m_webcamDescription;
  int m_webcamIndex    = -1;
  bool m_useDirectShow = true;
  int m_webcamWidth    = 0;
  int m_webcamHeight   = 0;

  void setOpacity(int opacity);
  int getOpacity() { return m_opacity; }
  void setUseScaledImages(bool on);
  bool getUseScaledImages() { return m_useScaledImages; }
  void setAlwaysLiveView(bool on);
  bool getAlwaysLiveView() { return m_alwaysLiveView; }
  void setBlackCapture(bool on);
  bool getBlackCapture() { return m_blackCapture; }
  void setPlaceOnXSheet(bool on);
  bool getPlaceOnXSheet() { return m_placeOnXSheet; }
  void setUseMjpg(bool on);
  bool getUseMjpg() { return m_useMjpg; }
  void setUseNumpadShortcuts(bool on);
  void setDrawBeneathLevels(bool on);
  bool getUseNumpadShortcuts() { return m_useNumpadShortcuts; }
  void toggleNumpadShortcuts(bool on);
  void toggleNumpadForFocusCheck(bool on);
  void setReviewTime(int time);
  int getReviewTime() { return m_reviewTime; }
  void setXSheetFrameNumber(int frameNumber);
  int getXSheetFrameNumber() { return m_xSheetFrameNumber; }
  void setFrameNumber(int frameNumber);
  int getFrameNumber() { return m_frameNumber; }
  void setLevelName(QString levelName);
  QString getLevelName() { return m_levelName; }
  void setFileType(QString fileType);
  QString getFileType() { return m_fileType; }
  void setFilePath(QString filePath);
  QString getFilePath() { return m_filePath; }
  void updateLevelNameAndFrame(std::wstring levelName);
  void setToNextNewLevel();
  void nextFrame();
  void previousFrame();
  void lastFrame();
  void nextName();
  void previousName();
  QStringList getAvailableSerialPorts();
  bool setSerialPort(QString port);
  void sendSerialData();
  void toggleInterval(bool on);
  void startInterval();
  void stopInterval();
  void setIntervalAmount(int value);
  void restartInterval();

  QString getFrameInfoText() { return m_frameInfoText; }
  QString getInfoColorName() { return m_infoColorName; }
  QString getFrameInfoToolTip() { return m_frameInfoToolTip; }
  bool toggleLiveView();
  void pauseLiveView();
  void getSubsampling();
  void setSubsampling();
  int getSubsamplingValue() { return m_subsampling; }
  void setSubsamplingValue(int subsampling);
  void captureImage();
  void postImportProcess();
  void refreshFrameInfo();
  void refreshCameraList();
  void changeCameras(int index);
  void raiseOpacity();
  void lowerOpacity();
  void jumpToCameraFrame();
  void removeStopMotionFrame();
  void disconnectAllCameras();

  std::string getTEnvCameraName();
  void setTEnvCameraName(std::string name);
  std::string getTEnvCameraResolution();
  void setTEnvCameraResolution(std::string resolution);

  void saveJpg(TRaster32P, TFilePath path);
  bool loadJpg(TFilePath path, TRaster32P& image);
  bool loadLineUpImage();
  void saveXmlFile();
  bool loadXmlFile();
  bool exportImageSequence();

  // Webcam Commands
  QList<QCameraInfo> getWebcams();
  QCamera* getWebcam() { return m_webcam; }
  void setWebcam(QCamera* camera);
  bool initWebcam(int index = 0);
  void releaseWebcam();
  void setWebcamResolution(QString resolution);
  void getWebcamImage();
  bool translateIndex(int index);
  bool getUseDirectShow() { return m_useDirectShow; }
  void setUseDirectShow(int state);
  QList<QSize> getWebcamResolutions() { return m_webcamResolutions; }
// void changeResolutions(int index);

// Canon Commands
#ifdef WITH_CANON
  void cameraAdded();
  void closeCanonSDK();
  int getCameraCount();
  std::string getCameraName();
  EdsError initializeCanonSDK();
  EdsCameraListRef getCameraList();
  EdsError releaseCameraList();
  EdsError getCamera(int index);
  EdsError releaseCamera();
  EdsError openCameraSession();
  EdsError closeCameraSession();
  EdsError downloadImage(EdsBaseRef object);
  EdsError takePicture();
  EdsError startCanonLiveView();
  EdsError endCanonLiveView();
  EdsError downloadEVFData();
  QStringList getIsoOptions() { return m_isoOptions; }
  QStringList getShutterSpeedOptions() { return m_shutterSpeedOptions; }
  QStringList getApertureOptions() { return m_apertureOptions; }
  QStringList getExposureOptions() { return m_exposureOptions; }
  QStringList getWhiteBalanceOptions() { return m_whiteBalanceOptions; }
  QStringList getColorTemperatureOptions() { return m_colorTempOptions; }
  QStringList getImageQualityOptions() { return m_imageQualityOptions; }
  QStringList getPictureStyleOptions() { return m_pictureStyleOptions; }
  EdsError getAvailableShutterSpeeds();
  EdsError getAvailableIso();
  EdsError getAvailableApertures();
  EdsError getAvailableExposureCompensations();
  EdsError getAvailableWhiteBalances();
  EdsError getAvailableImageQualities();
  EdsError getAvailablePictureStyles();
  void buildColorTemperatures();
  QString getCurrentShutterSpeed();
  QString getCurrentIso();
  QString getCurrentAperture();
  QString getCurrentExposureCompensation();
  QString getCurrentWhiteBalance();
  QString getCurrentColorTemperature();
  QString getCurrentImageQuality();
  QString getCurrentPictureStyle();
  QString getCurrentBatteryLevel();
  EdsError setShutterSpeed(QString shutterSpeed);
  EdsError setIso(QString iso);
  EdsError setAperture(QString aperture);
  EdsError setExposureCompensation(QString exposure);
  EdsError setWhiteBalance(QString whiteBalance);
  EdsError setColorTemperature(QString temp);
  EdsError setImageQuality(QString quality);
  EdsError setPictureStyle(QString style);
  QString getMode();
  void refreshOptions();
  EdsError zoomLiveView();
  EdsError setZoomPoint();
  void makeZoomPoint(TPointD pos);
  void toggleZoomPicking();
  void calculateZoomPoint();
  EdsError focusNear();
  EdsError focusFar();
  EdsError focusNear2();
  EdsError focusFar2();
  EdsError focusNear3();
  EdsError focusFar3();
  void extendCameraOnTime();
#endif

public slots:
  void onImageReady(const bool&);
  void onFinished();
  void onTimeout();
  void onReviewTimeout();
  void update();
  bool importImage();
  void onSceneSwitched();
  void onPlaybackChanged();
  void onIntervalCaptureTimerTimeout();

signals:
  void newLiveViewImageReady();
  void liveViewStopped();
  void newImageReady();
  void cameraChanged(QString);
  void optionsChanged();
  void liveViewChanged(bool);
  void newCameraSelected(int, bool);
  void webcamResolutionsChanged();
  void newWebcamResolutionSelected(int);

  // canon signals
  void apertureOptionsChanged();
  void isoOptionsChanged();
  void shutterSpeedOptionsChanged();
  void exposureOptionsChanged();
  void whiteBalanceOptionsChanged();
  void colorTemperatureChanged();
  void imageQualityOptionsChanged();
  void pictureStyleOptionsChanged();
  void apertureChangedSignal(QString);
  void isoChangedSignal(QString);
  void shutterSpeedChangedSignal(QString);
  void exposureChangedSignal(QString);
  void whiteBalanceChangedSignal(QString);
  void colorTemperatureChangedSignal(QString);
  void imageQualityChangedSignal(QString);
  void pictureStyleChangedSignal(QString);
  void modeChanged();
  void focusCheckToggled(bool);
  void pickFocusCheckToggled(bool);

  void newDimensions();
  void subsamplingChanged(int);
  void filePathChanged(QString);
  void levelNameChanged(QString);
  void fileTypeChanged(QString);
  void xSheetFrameNumberChanged(int);
  void frameNumberChanged(int);
  void frameInfoTextChanged(QString);
  void opacityChanged(int);
  void scaleFullSizeImagesSignal(bool);
  void blackCaptureSignal(bool);
  void liveViewOnAllFramesSignal(bool);
  void placeOnXSheetSignal(bool);
  void useMjpgSignal(bool);
  void useNumpadSignal(bool);
  void drawBeneathLevelsSignal(bool);
  void useDirectShowSignal(bool);
  void reviewTimeChangedSignal(int);
  void updateCameraList(QString);
  void intervalToggled(bool);
  void intervalStarted();
  void intervalStopped();
  void intervalAmountChanged(int);
};

#endif  // STOPMOTION_H