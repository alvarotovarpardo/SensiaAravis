#ifndef SENSIA_BRIDGE_H
#define SENSIA_BRIDGE_H

#include <arv.h>
#include <glib.h>
#include <map>
#include <string>
#include <cstdlib>
#include <iostream>

typedef struct {
    char *name;
    ArvDevice *device;
    ArvGc *genicam;

    gboolean has_serial_number;

    gboolean has_gain;
    gboolean gain_raw_as_float;
    gboolean gain_abs_as_float;

    gboolean has_brightness;
    gboolean has_black_level_raw;
    gboolean has_black_level;

    gboolean has_exposure_time;
    gboolean has_acquisition_frame_rate;
    gboolean has_acquisition_frame_rate_auto;
    gboolean has_acquisition_frame_rate_enabled;

    gboolean has_region_offset;

    GError *init_error;
} ArvCameraPrivate;

namespace proyectoSensiaBridge {

    class SensiaBridge {
    public:
        void doSomething(const std::map<std::string, std::string>& arguments);
    };
    ARV_API ArvBuffer * sensia_camera_acquisition (ArvCamera *camera, guint64 timeout, GError **error);
    void open_device(ArvCameraPrivate *camera_priv, const char *objetivoDeviceId);
    int Saca_FOTOS (ArvCameraPrivate *cam);
    void set_camera_feature(ArvDevice *device, int gain_value, const char* Gain_SensorType);

}

#endif // SENSIA_BRIDGE_H