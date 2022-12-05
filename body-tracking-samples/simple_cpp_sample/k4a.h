#ifndef K4A_H
 #define K4A_H
 
 #ifdef __cplusplus
 #include <cinttypes>
 #else
 #include <inttypes.h>
 #endif
 #include <k4a/k4aversion.h>
 #include <k4a/k4atypes.h>
 #include <k4a/k4a_export.h>
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 K4A_EXPORT uint32_t k4a_device_get_installed_count(void);
 
 K4A_EXPORT k4a_result_t k4a_set_debug_message_handler(k4a_logging_message_cb_t *message_cb,
                                                       void *message_cb_context,
                                                       k4a_log_level_t min_level);
 
 K4A_EXPORT k4a_result_t k4a_set_allocator(k4a_memory_allocate_cb_t allocate, k4a_memory_destroy_cb_t free);
 
 K4A_EXPORT k4a_result_t k4a_device_open(uint32_t index, k4a_device_t *device_handle);
 
 K4A_EXPORT void k4a_device_close(k4a_device_t device_handle);
 
 K4A_EXPORT k4a_wait_result_t k4a_device_get_capture(k4a_device_t device_handle,
                                                     k4a_capture_t *capture_handle,
                                                     int32_t timeout_in_ms);
 
 K4A_EXPORT k4a_wait_result_t k4a_device_get_imu_sample(k4a_device_t device_handle,
                                                        k4a_imu_sample_t *imu_sample,
                                                        int32_t timeout_in_ms);
 
 K4A_EXPORT k4a_result_t k4a_capture_create(k4a_capture_t *capture_handle);
 
 K4A_EXPORT void k4a_capture_release(k4a_capture_t capture_handle);
 
 K4A_EXPORT void k4a_capture_reference(k4a_capture_t capture_handle);
 
 K4A_EXPORT k4a_image_t k4a_capture_get_color_image(k4a_capture_t capture_handle);
 
 K4A_EXPORT k4a_image_t k4a_capture_get_depth_image(k4a_capture_t capture_handle);
 
 K4A_EXPORT k4a_image_t k4a_capture_get_ir_image(k4a_capture_t capture_handle);
 
 K4A_EXPORT void k4a_capture_set_color_image(k4a_capture_t capture_handle, k4a_image_t image_handle);
 
 K4A_EXPORT void k4a_capture_set_depth_image(k4a_capture_t capture_handle, k4a_image_t image_handle);
 
 K4A_EXPORT void k4a_capture_set_ir_image(k4a_capture_t capture_handle, k4a_image_t image_handle);
 
 K4A_EXPORT void k4a_capture_set_temperature_c(k4a_capture_t capture_handle, float temperature_c);
 
 K4A_EXPORT float k4a_capture_get_temperature_c(k4a_capture_t capture_handle);
 
 K4A_EXPORT k4a_result_t k4a_image_create(k4a_image_format_t format,
                                          int width_pixels,
                                          int height_pixels,
                                          int stride_bytes,
                                          k4a_image_t *image_handle);
 
 K4A_EXPORT k4a_result_t k4a_image_create_from_buffer(k4a_image_format_t format,
                                                      int width_pixels,
                                                      int height_pixels,
                                                      int stride_bytes,
                                                      uint8_t *buffer,
                                                      size_t buffer_size,
                                                      k4a_memory_destroy_cb_t *buffer_release_cb,
                                                      void *buffer_release_cb_context,
                                                      k4a_image_t *image_handle);
 
 K4A_EXPORT uint8_t *k4a_image_get_buffer(k4a_image_t image_handle);
 
 K4A_EXPORT size_t k4a_image_get_size(k4a_image_t image_handle);
 
 K4A_EXPORT k4a_image_format_t k4a_image_get_format(k4a_image_t image_handle);
 
 K4A_EXPORT int k4a_image_get_width_pixels(k4a_image_t image_handle);
 
 K4A_EXPORT int k4a_image_get_height_pixels(k4a_image_t image_handle);
 
 K4A_EXPORT int k4a_image_get_stride_bytes(k4a_image_t image_handle);
 
 K4A_DEPRECATED_EXPORT uint64_t k4a_image_get_timestamp_usec(k4a_image_t image_handle);
 
 K4A_EXPORT uint64_t k4a_image_get_device_timestamp_usec(k4a_image_t image_handle);
 
 K4A_EXPORT uint64_t k4a_image_get_system_timestamp_nsec(k4a_image_t image_handle);
 
 K4A_EXPORT uint64_t k4a_image_get_exposure_usec(k4a_image_t image_handle);
 
 K4A_EXPORT uint32_t k4a_image_get_white_balance(k4a_image_t image_handle);
 
 K4A_EXPORT uint32_t k4a_image_get_iso_speed(k4a_image_t image_handle);
 
 K4A_EXPORT void k4a_image_set_device_timestamp_usec(k4a_image_t image_handle, uint64_t timestamp_usec);
 
 K4A_DEPRECATED_EXPORT void k4a_image_set_timestamp_usec(k4a_image_t image_handle, uint64_t timestamp_usec);
 
 K4A_EXPORT void k4a_image_set_system_timestamp_nsec(k4a_image_t image_handle, uint64_t timestamp_nsec);
 
 K4A_EXPORT void k4a_image_set_exposure_usec(k4a_image_t image_handle, uint64_t exposure_usec);
 
 K4A_DEPRECATED_EXPORT void k4a_image_set_exposure_time_usec(k4a_image_t image_handle, uint64_t exposure_usec);
 
 K4A_EXPORT void k4a_image_set_white_balance(k4a_image_t image_handle, uint32_t white_balance);
 
 K4A_EXPORT void k4a_image_set_iso_speed(k4a_image_t image_handle, uint32_t iso_speed);
 
 K4A_EXPORT void k4a_image_reference(k4a_image_t image_handle);
 
 K4A_EXPORT void k4a_image_release(k4a_image_t image_handle);
 
 K4A_EXPORT k4a_result_t k4a_device_start_cameras(k4a_device_t device_handle, const k4a_device_configuration_t *config);
 
 K4A_EXPORT void k4a_device_stop_cameras(k4a_device_t device_handle);
 
 K4A_EXPORT k4a_result_t k4a_device_start_imu(k4a_device_t device_handle);
 
 K4A_EXPORT void k4a_device_stop_imu(k4a_device_t device_handle);
 
 K4A_EXPORT k4a_buffer_result_t k4a_device_get_serialnum(k4a_device_t device_handle,
                                                         char *serial_number,
                                                         size_t *serial_number_size);
 
 K4A_EXPORT k4a_result_t k4a_device_get_version(k4a_device_t device_handle, k4a_hardware_version_t *version);
 
 K4A_EXPORT k4a_result_t k4a_device_get_color_control_capabilities(k4a_device_t device_handle,
                                                                   k4a_color_control_command_t command,
                                                                   bool *supports_auto,
                                                                   int32_t *min_value,
                                                                   int32_t *max_value,
                                                                   int32_t *step_value,
                                                                   int32_t *default_value,
                                                                   k4a_color_control_mode_t *default_mode);
 
 K4A_EXPORT k4a_result_t k4a_device_get_color_control(k4a_device_t device_handle,
                                                      k4a_color_control_command_t command,
                                                      k4a_color_control_mode_t *mode,
                                                      int32_t *value);
 
 K4A_EXPORT k4a_result_t k4a_device_set_color_control(k4a_device_t device_handle,
                                                      k4a_color_control_command_t command,
                                                      k4a_color_control_mode_t mode,
                                                      int32_t value);
 
 K4A_EXPORT k4a_buffer_result_t k4a_device_get_raw_calibration(k4a_device_t device_handle,
                                                               uint8_t *data,
                                                               size_t *data_size);
 
 K4A_EXPORT k4a_result_t k4a_device_get_calibration(k4a_device_t device_handle,
                                                    const k4a_depth_mode_t depth_mode,
                                                    const k4a_color_resolution_t color_resolution,
                                                    k4a_calibration_t *calibration);
 
 K4A_EXPORT k4a_result_t k4a_device_get_sync_jack(k4a_device_t device_handle,
                                                  bool *sync_in_jack_connected,
                                                  bool *sync_out_jack_connected);
 
 K4A_EXPORT k4a_result_t k4a_calibration_get_from_raw(char *raw_calibration,
                                                      size_t raw_calibration_size,
                                                      const k4a_depth_mode_t depth_mode,
                                                      const k4a_color_resolution_t color_resolution,
                                                      k4a_calibration_t *calibration);
 
 K4A_EXPORT k4a_result_t k4a_calibration_3d_to_3d(const k4a_calibration_t *calibration,
                                                  const k4a_float3_t *source_point3d_mm,
                                                  const k4a_calibration_type_t source_camera,
                                                  const k4a_calibration_type_t target_camera,
                                                  k4a_float3_t *target_point3d_mm);
 
 K4A_EXPORT k4a_result_t k4a_calibration_2d_to_3d(const k4a_calibration_t *calibration,
                                                  const k4a_float2_t *source_point2d,
                                                  const float source_depth_mm,
                                                  const k4a_calibration_type_t source_camera,
                                                  const k4a_calibration_type_t target_camera,
                                                  k4a_float3_t *target_point3d_mm,
                                                  int *valid);
 
 K4A_EXPORT k4a_result_t k4a_calibration_3d_to_2d(const k4a_calibration_t *calibration,
                                                  const k4a_float3_t *source_point3d_mm,
                                                  const k4a_calibration_type_t source_camera,
                                                  const k4a_calibration_type_t target_camera,
                                                  k4a_float2_t *target_point2d,
                                                  int *valid);
 
 K4A_EXPORT k4a_result_t k4a_calibration_2d_to_2d(const k4a_calibration_t *calibration,
                                                  const k4a_float2_t *source_point2d,
                                                  const float source_depth_mm,
                                                  const k4a_calibration_type_t source_camera,
                                                  const k4a_calibration_type_t target_camera,
                                                  k4a_float2_t *target_point2d,
                                                  int *valid);
 
 K4A_EXPORT k4a_result_t k4a_calibration_color_2d_to_depth_2d(const k4a_calibration_t *calibration,
                                                              const k4a_float2_t *source_point2d,
                                                              const k4a_image_t depth_image,
                                                              k4a_float2_t *target_point2d,
                                                              int *valid);
 
 K4A_EXPORT k4a_transformation_t k4a_transformation_create(const k4a_calibration_t *calibration);
 
 K4A_EXPORT void k4a_transformation_destroy(k4a_transformation_t transformation_handle);
 
 K4A_EXPORT k4a_result_t k4a_transformation_depth_image_to_color_camera(k4a_transformation_t transformation_handle,
                                                                        const k4a_image_t depth_image,
                                                                        k4a_image_t transformed_depth_image);
 
 K4A_EXPORT k4a_result_t
 k4a_transformation_depth_image_to_color_camera_custom(k4a_transformation_t transformation_handle,
                                                       const k4a_image_t depth_image,
                                                       const k4a_image_t custom_image,
                                                       k4a_image_t transformed_depth_image,
                                                       k4a_image_t transformed_custom_image,
                                                       k4a_transformation_interpolation_type_t interpolation_type,
                                                       uint32_t invalid_custom_value);
 
 K4A_EXPORT k4a_result_t k4a_transformation_color_image_to_depth_camera(k4a_transformation_t transformation_handle,
                                                                        const k4a_image_t depth_image,
                                                                        const k4a_image_t color_image,
                                                                        k4a_image_t transformed_color_image);
 
 K4A_EXPORT k4a_result_t k4a_transformation_depth_image_to_point_cloud(k4a_transformation_t transformation_handle,
                                                                       const k4a_image_t depth_image,
                                                                       const k4a_calibration_type_t camera,
                                                                       k4a_image_t xyz_image);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* K4A_H */