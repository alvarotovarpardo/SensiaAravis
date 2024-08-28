#include "SensiaBridge.h"

void proyectoSensiaBridge::open_device(ArvCameraPrivate *camera_priv, const char *objetivoDeviceId) {
    // Inicializar el error a nulo
    GError *error = nullptr;

    // Intentar abrir el dispositivo
    camera_priv->device = arv_open_device(objetivoDeviceId, &error);

    // Verificar si ocurrió un error
    if (error != nullptr) {
        // Limpiar cualquier error previo
        if (camera_priv->init_error != nullptr) {
            g_clear_error(&camera_priv->init_error);
        }
        // Almacenar el nuevo error en la estructura
        camera_priv->init_error = error;
    }
}

namespace proyectoSensiaBridge {

    const std::string SENSIA_NAME_U3V = "SENSIA-U3V";
    const std::string SENSIA_VENDOR = "5353";
    const std::string SENSIA_NAME_DEVICE = "SENSIA-U3V";

    const std::string GAIN_SENSORTYPE2 = "Gain_SensorType2";
    const std::string GAIN_SENSORTYPE6 = "Gain_SensorType6";
    const std::string GSK_SENSORTYPE8 = "GSK_SensorType8";
    const std::string GSK_SENSORTYPE6 = "GSK_SensorType6";

    const std::string GAIN_REGISTER_KEY = "GainRegister";
    const std::string GENICAM_XML = "genicamSensia.xml";
    const std::string SENSIA_LOG_FILE = "Sensia_Log.file";
    const std::string TEST_NAME_FILE = "SoyEl.xml";

    void SensiaBridge::doSomething(const std::map<std::string, std::string>& arguments) {

        std::string strU3v = SENSIA_NAME_U3V;
        std::string strnameDispositivo = SENSIA_NAME_DEVICE;
        const char *objetivoDeviceId = nullptr;
        ArvBuffer *buffer = NULL;
        GError *errorStream = nullptr;
        const char *test_name = TEST_NAME_FILE.c_str();
        const char* GenICamXML = GENICAM_XML.c_str();
        const char* SENSIA_Log = SENSIA_LOG_FILE.c_str();

        FILE *file = fopen(GenICamXML, "w");
        FILE *logFile = fopen(SENSIA_Log, "w");
        FILE *filexml2 = fopen(test_name, "w");

        // Inicializar la estructura dinámica
        ArvCameraPrivate *priv = g_new0(ArvCameraPrivate, 1);
        std::cout << "inicio aplicacion SensiaBridge." << std::endl;
        arv_update_device_list();

        // Obtener el número de dispositivos disponibles
        int num_devices = arv_get_n_devices();

        if (num_devices == 0) {
            std::cout << "No se encontraron dispositivos." << std::endl;
        } else {
            std::cout << "Número de dispositivos encontrados: " << num_devices << std::endl;
            for (int i = 0; i < num_devices; i++) {
                // Obtener y mostrar la información de cada dispositivo
                const char *device_id = arv_get_device_id(i);
                const char *device_model = arv_get_device_model(i);
                const char *device_serial = arv_get_device_serial_nbr(i);
                const char *device_vendor = arv_get_device_vendor(i);
                const char *device_manufacturer = arv_get_device_manufacturer_info(i);

                std::cout << "Dispositivo " << i + 1 << ": " << device_id << " modelo: " << device_model << std::endl;
                std::cout << "Marca " << device_vendor << " Serial NO: " << device_serial << " manufacturer: " << device_manufacturer << std::endl;
                std::string str(device_model);

                if (str == strU3v) {
                    std::cout << "ERES MI OBJETIVO!!!!! " << std::endl;
                    objetivoDeviceId = device_id;
                    std::string str2(device_serial);
                    strnameDispositivo = str2;
                }
            }
            try {
                std::cout << "PREVIO A OPEN DEVICE... " << std::endl;

                proyectoSensiaBridge::open_device(priv, objetivoDeviceId);

                if (priv->init_error != nullptr) {
                    std::cout << "Error (Gerror): " << priv->init_error->message << std::endl;
                    g_clear_error(&priv->init_error);
                } else {
                    std::cout << "Device opened successfully" << std::endl;
                    const char *genicam;
                    size_t size;
                    try{
                        genicam = arv_device_get_genicam_xml (priv->device, &size);
                        if (genicam != NULL) {
                            fprintf(file, "%*s\n", (int)size, genicam);
                            fclose(file);
                            printf("El contenido XML se ha guardado en %s\n",  GenICamXML);
                        }
                        auto it = arguments.find(GSK_SENSORTYPE6);
                        if (it != arguments.end()) {
                            std::cout << "Estamos llamando a '" << it->first << "' con el valor: " << it->second << std::endl;
                            int value = std::stoi(it->second);
                            const char *key = it->first.c_str();
                            set_camera_feature(priv->device, value, key);
                        } else {
                            std::cout << "La clave '" << it->first << "' no esta presente. nunca aparecera esto... y si aparece es que me he olvidado de poner en el if la cadena de texto..." << std::endl;
                        }
                        Saca_FOTOS(priv);

                        /* Segunda linea de xml para comparar luego */
                        genicam = arv_device_get_genicam_xml (priv->device, &size);
                        if (genicam != NULL) {
                            fprintf(file, "%*s\n", (int)size, test_name);
                            fclose(filexml2);
                            printf("El contenido segundo XML se ha guardado en %s\n",  test_name);
                        }

                    } catch (...) {
                        std::cout << "Error al obtener el xml! Catch de genicam = arv_device_get_genicam_xml (priv->device, &size)" << std::endl;
                    }
                }

            } catch (...) {
                std::cout << "Estamos en el catch de proyectoSensiaBridge::open_device(priv, objetivoDeviceId) " << std::endl;
            }

        }

        fclose(logFile);
        // Liberar la estructura dinámica
        g_free(priv);
    }


    ArvBuffer * sensia_camera_acquisition (ArvCamera *camera, guint64 timeout, GError **error)
    {
        GError *local_error = NULL;
        ArvStream *stream;
        ArvBuffer *buffer = NULL;
        gint payload;

        g_return_val_if_fail (ARV_IS_CAMERA (camera), NULL);

        stream = arv_camera_create_stream (camera, NULL, NULL, &local_error);
        if (ARV_IS_STREAM(stream)) {
            payload = arv_camera_get_payload (camera, &local_error);
            if (local_error == NULL) {
                arv_stream_push_buffer (stream,  arv_buffer_new (payload, NULL));
                arv_camera_set_acquisition_mode (camera, ARV_ACQUISITION_MODE_SINGLE_FRAME, &local_error);
                            if (local_error != NULL &&
                                local_error->code == ARV_GC_ERROR_ENUM_ENTRY_NOT_FOUND) {
                                    g_clear_error (&local_error);
                                    /* Some cameras don't support SingleFrame, fall back to Continuous */
                                    arv_camera_set_acquisition_mode (camera, ARV_ACQUISITION_MODE_CONTINUOUS, &local_error);
                            }
            }
            if (local_error == NULL)
                arv_camera_start_acquisition (camera, &local_error);
            if (local_error == NULL) {
                if (timeout > 0)
                    buffer = arv_stream_timeout_pop_buffer (stream, timeout);
                else
                    buffer = arv_stream_pop_buffer (stream);
                arv_camera_stop_acquisition (camera, &local_error);
            }

            g_object_unref (stream);
        }

        if (local_error != NULL)
            g_propagate_error (error, local_error);

        return buffer;
    }



    int Saca_FOTOS (ArvCameraPrivate *cam)
    {
        ArvCamera *camera;
        ArvStream *stream;
        ArvBuffer *buffer;
        GError *error = nullptr;

        // Inicializar Aravis
        arv_update_device_list();

        // Abrir la cámara
        camera = arv_camera_new_with_device(cam->device, &error);

        if (camera == NULL) {
            g_error("No se pudo abrir la cámara\n");
            return -1;
        }

        // Crear el flujo de video
        stream = arv_camera_create_stream (camera, NULL, NULL, &error);
        if (stream == NULL) {
            g_error("No se pudo crear el stream\n");
            g_object_unref(camera);
            return -1;
        }

        // Comenzar la adquisición de imágenes
        arv_camera_start_acquisition (camera, &error);

        // Capturar y procesar algunas imágenes
        for (int i = 0; i < 10; i++) {
            buffer = arv_stream_timeout_pop_buffer (stream, 2000000);
            if (buffer != NULL) {
                if (arv_buffer_get_status (buffer) == ARV_BUFFER_STATUS_SUCCESS) {
                    g_print("Imagen capturada: %d x %d\n",
                            arv_buffer_get_image_width (buffer),
                            arv_buffer_get_image_height (buffer));
                }
                arv_stream_push_buffer (stream, buffer);
            }
        }

        // Finalizar la adquisición
        arv_camera_stop_acquisition (camera, &error);

        // Liberar recursos
        g_object_unref(stream);
        g_object_unref(camera);

        return 0;
    }


    void set_camera_feature(ArvDevice *device, int gain_value, const char* Gain_SensorType) {

        if (device == NULL)
            fprintf(stderr, "Estoy viendo el dispositivo a vacio.\n");

        // Obtener el nodo de Sensor del dispositivo
        ArvGcFeatureNode *gain_node = ARV_GC_FEATURE_NODE(arv_device_get_feature(device, Gain_SensorType));

        // Verificar si el nodo de sensor fue encontrado
        if (gain_node == NULL) {
            fprintf(stderr, "No se encontro el nodo de sensor.\n");
            return;
        }

        // Comprobar si el nodo de ganancia es de tipo flotante (float)
        if (ARV_IS_GC_INTEGER(gain_node)) {
            // Establecer el valor de la ganancia
            GError *error = NULL;
            arv_gc_integer_set_value(ARV_GC_INTEGER(gain_node), gain_value, &error);

            // Manejar posibles errores
            if (error != NULL) {
                fprintf(stderr, "Error al establecer la sensor: %s\n", error->message);
                g_clear_error(&error);
            } else {
                printf("Sensor establecida en %d\n", gain_value);
            }
        } else {
            fprintf(stderr, "El nodo de sensor no es de tipo flotante.\n");
        }
    }


}