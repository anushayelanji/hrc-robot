#ifndef K4A_HPP
 #define K4A_HPP
 
 #include "k4a.h"
 
 #include <algorithm>
 #include <chrono>
 #include <cstdint>
 #include <limits>
 #include <stdexcept>
 #include <string>
 #include <vector>
 
 namespace k4a
 {
 
 class error : public std::runtime_error
 {
 public:
     using runtime_error::runtime_error;
 };
 
 // Helper functions not intended for use by client code
 //
 namespace internal
 {
 
 template<typename output_type, typename input_type> output_type clamp_cast(input_type input)
 {
     static_assert(std::is_arithmetic<input_type>::value, "clamp_cast only supports arithmetic types");
     static_assert(std::is_arithmetic<output_type>::value, "clamp_cast only supports arithmetic types");
     const input_type min_value = std::is_signed<input_type>() ?
                                      static_cast<input_type>(std::numeric_limits<output_type>::min()) :
                                      0;
 
     input_type max_value = static_cast<input_type>(std::numeric_limits<output_type>::max());
     if (max_value < 0)
     {
         // Output type is of greater or equal size to input type and we've overflowed.
         //
         max_value = std::numeric_limits<input_type>::max();
     }
     input = std::min(input, max_value);
     input = std::max(input, min_value);
     return static_cast<output_type>(input);
 }
 } // namespace internal
 
 class image
 {
 public:
     image(k4a_image_t handle = nullptr) noexcept : m_handle(handle) {}
 
     image(const image &other) noexcept : m_handle(other.m_handle)
     {
         if (m_handle != nullptr)
         {
             k4a_image_reference(m_handle);
         }
     }
 
     image(image &&other) noexcept : m_handle(other.m_handle)
     {
         other.m_handle = nullptr;
     }
 
     ~image()
     {
         reset();
     }
 
     image &operator=(const image &other) noexcept
     {
         if (this != &other)
         {
             reset();
             m_handle = other.m_handle;
             if (m_handle != nullptr)
             {
                 k4a_image_reference(m_handle);
             }
         }
         return *this;
     }
 
     image &operator=(image &&other) noexcept
     {
         if (this != &other)
         {
             reset();
             m_handle = other.m_handle;
             other.m_handle = nullptr;
         }
         return *this;
     }
 
     image &operator=(std::nullptr_t) noexcept
     {
         reset();
         return *this;
     }
 
     bool operator==(const image &other) const noexcept
     {
         return m_handle == other.m_handle;
     }
 
     bool operator==(std::nullptr_t) const noexcept
     {
         return m_handle == nullptr;
     }
 
     bool operator!=(const image &other) const noexcept
     {
         return m_handle != other.m_handle;
     }
 
     bool operator!=(std::nullptr_t) const noexcept
     {
         return m_handle != nullptr;
     }
 
     explicit operator bool() const noexcept
     {
         return is_valid();
     }
 
     bool is_valid() const noexcept
     {
         return m_handle != nullptr;
     }
 
     k4a_image_t handle() const noexcept
     {
         return m_handle;
     }
 
     void reset() noexcept
     {
         if (m_handle != nullptr)
         {
             k4a_image_release(m_handle);
             m_handle = nullptr;
         }
     }
 
     static image create(k4a_image_format_t format, int width_pixels, int height_pixels, int stride_bytes)
     {
         k4a_image_t handle = nullptr;
         k4a_result_t result = k4a_image_create(format, width_pixels, height_pixels, stride_bytes, &handle);
         if (K4A_RESULT_SUCCEEDED != result)
         {
             throw error("Failed to create image!");
         }
         return image(handle);
     }
 
     static image create_from_buffer(k4a_image_format_t format,
                                     int width_pixels,
                                     int height_pixels,
                                     int stride_bytes,
                                     uint8_t *buffer,
                                     size_t buffer_size,
                                     k4a_memory_destroy_cb_t *buffer_release_cb,
                                     void *buffer_release_cb_context)
     {
         k4a_image_t handle = nullptr;
         k4a_result_t result = k4a_image_create_from_buffer(format,
                                                            width_pixels,
                                                            height_pixels,
                                                            stride_bytes,
                                                            buffer,
                                                            buffer_size,
                                                            buffer_release_cb,
                                                            buffer_release_cb_context,
                                                            &handle);
         if (K4A_RESULT_SUCCEEDED != result)
         {
             throw error("Failed to create image from buffer");
         }
         return image(handle);
     }
 
     uint8_t *get_buffer() noexcept
     {
         return k4a_image_get_buffer(m_handle);
     }
 
     const uint8_t *get_buffer() const noexcept
     {
         return k4a_image_get_buffer(m_handle);
     }
 
     size_t get_size() const noexcept
     {
         return k4a_image_get_size(m_handle);
     }
 
     k4a_image_format_t get_format() const noexcept
     {
         return k4a_image_get_format(m_handle);
     }
 
     int get_width_pixels() const noexcept
     {
         return k4a_image_get_width_pixels(m_handle);
     }
 
     int get_height_pixels() const noexcept
     {
         return k4a_image_get_height_pixels(m_handle);
     }
 
     int get_stride_bytes() const noexcept
     {
         return k4a_image_get_stride_bytes(m_handle);
     }
 
     std::chrono::microseconds get_device_timestamp() const noexcept
     {
         return std::chrono::microseconds(k4a_image_get_device_timestamp_usec(m_handle));
     }
 
     std::chrono::nanoseconds get_system_timestamp() const noexcept
     {
         return std::chrono::nanoseconds(k4a_image_get_system_timestamp_nsec(m_handle));
     }
 
     std::chrono::microseconds get_exposure() const noexcept
     {
         return std::chrono::microseconds(k4a_image_get_exposure_usec(m_handle));
     }
 
     uint32_t get_white_balance() const noexcept
     {
         return k4a_image_get_white_balance(m_handle);
     }
 
     uint32_t get_iso_speed() const noexcept
     {
         return k4a_image_get_iso_speed(m_handle);
     }
 
     void set_timestamp(std::chrono::microseconds timestamp) noexcept
     {
         k4a_image_set_device_timestamp_usec(m_handle, internal::clamp_cast<uint64_t>(timestamp.count()));
     }
 
     void set_exposure_time(std::chrono::microseconds exposure) noexcept
     {
         k4a_image_set_exposure_usec(m_handle, internal::clamp_cast<uint64_t>(exposure.count()));
     }
 
     void set_white_balance(uint32_t white_balance) noexcept
     {
         k4a_image_set_white_balance(m_handle, white_balance);
     }
 
     void set_iso_speed(uint32_t iso_speed) noexcept
     {
         k4a_image_set_iso_speed(m_handle, iso_speed);
     }
 
 private:
     k4a_image_t m_handle;
 };
 
 class capture
 {
 public:
     capture(k4a_capture_t handle = nullptr) noexcept : m_handle(handle) {}
 
     capture(const capture &other) noexcept : m_handle(other.m_handle)
     {
         if (m_handle != nullptr)
         {
             k4a_capture_reference(m_handle);
         }
     }
 
     capture(capture &&other) noexcept : m_handle(other.m_handle)
     {
         other.m_handle = nullptr;
     }
 
     ~capture()
     {
         reset();
     }
 
     capture &operator=(const capture &other) noexcept
     {
         if (this != &other)
         {
             reset();
             m_handle = other.m_handle;
             if (m_handle != nullptr)
             {
                 k4a_capture_reference(m_handle);
             }
         }
         return *this;
     }
 
     capture &operator=(capture &&other) noexcept
     {
         if (this != &other)
         {
             reset();
             m_handle = other.m_handle;
             other.m_handle = nullptr;
         }
         return *this;
     }
 
     capture &operator=(std::nullptr_t) noexcept
     {
         reset();
         return *this;
     }
 
     bool operator==(const capture &other) const noexcept
     {
         return m_handle == other.m_handle;
     }
 
     bool operator==(std::nullptr_t) const noexcept
     {
         return m_handle == nullptr;
     }
 
     bool operator!=(const capture &other) const noexcept
     {
         return m_handle != other.m_handle;
     }
 
     bool operator!=(std::nullptr_t) const noexcept
     {
         return m_handle != nullptr;
     }
 
     explicit operator bool() const noexcept
     {
         return is_valid();
     }
 
     bool is_valid() const noexcept
     {
         return m_handle != nullptr;
     }
 
     k4a_capture_t handle() const noexcept
     {
         return m_handle;
     }
 
     void reset() noexcept
     {
         if (m_handle != nullptr)
         {
             k4a_capture_release(m_handle);
             m_handle = nullptr;
         }
     }
 
     image get_color_image() const noexcept
     {
         return image(k4a_capture_get_color_image(m_handle));
     }
 
     image get_depth_image() const noexcept
     {
         return image(k4a_capture_get_depth_image(m_handle));
     }
 
     image get_ir_image() const noexcept
     {
         return image(k4a_capture_get_ir_image(m_handle));
     }
 
     void set_color_image(const image &color_image) noexcept
     {
         k4a_capture_set_color_image(m_handle, color_image.handle());
     }
 
     void set_depth_image(const image &depth_image) noexcept
     {
         k4a_capture_set_depth_image(m_handle, depth_image.handle());
     }
 
     void set_ir_image(const image &ir_image) noexcept
     {
         k4a_capture_set_ir_image(m_handle, ir_image.handle());
     }
 
     void set_temperature_c(float temperature_c) noexcept
     {
         k4a_capture_set_temperature_c(m_handle, temperature_c);
     }
 
     float get_temperature_c() const noexcept
     {
         return k4a_capture_get_temperature_c(m_handle);
     }
 
     static capture create()
     {
         k4a_capture_t handle = nullptr;
         k4a_result_t result = k4a_capture_create(&handle);
         if (K4A_RESULT_SUCCEEDED != result)
         {
             throw error("Failed to create capture!");
         }
         return capture(handle);
     }
 
 private:
     k4a_capture_t m_handle;
 };
 
 struct calibration : public k4a_calibration_t
 {
     k4a_float3_t convert_3d_to_3d(const k4a_float3_t &source_point3d,
                                   k4a_calibration_type_t source_camera,
                                   k4a_calibration_type_t target_camera) const
     {
         k4a_float3_t target_point3d;
         k4a_result_t result =
             k4a_calibration_3d_to_3d(this, &source_point3d, source_camera, target_camera, &target_point3d);
 
         if (K4A_RESULT_SUCCEEDED != result)
         {
             throw error("Calibration contained invalid transformation parameters!");
         }
         return target_point3d;
     }
 
     bool convert_2d_to_3d(const k4a_float2_t &source_point2d,
                           float source_depth,
                           k4a_calibration_type_t source_camera,
                           k4a_calibration_type_t target_camera,
                           k4a_float3_t *target_point3d) const
     {
         int valid = 0;
         k4a_result_t result = k4a_calibration_2d_to_3d(
             this, &source_point2d, source_depth, source_camera, target_camera, target_point3d, &valid);
 
         if (K4A_RESULT_SUCCEEDED != result)
         {
             throw error("Calibration contained invalid transformation parameters!");
         }
         return static_cast<bool>(valid);
     }
 
     bool convert_3d_to_2d(const k4a_float3_t &source_point3d,
                           k4a_calibration_type_t source_camera,
                           k4a_calibration_type_t target_camera,
                           k4a_float2_t *target_point2d) const
     {
         int valid = 0;
         k4a_result_t result =
             k4a_calibration_3d_to_2d(this, &source_point3d, source_camera, target_camera, target_point2d, &valid);
 
         if (K4A_RESULT_SUCCEEDED != result)
         {
             throw error("Calibration contained invalid transformation parameters!");
         }
         return static_cast<bool>(valid);
     }
 
     bool convert_2d_to_2d(const k4a_float2_t &source_point2d,
                           float source_depth,
                           k4a_calibration_type_t source_camera,
                           k4a_calibration_type_t target_camera,
                           k4a_float2_t *target_point2d) const
     {
         int valid = 0;
         k4a_result_t result = k4a_calibration_2d_to_2d(
             this, &source_point2d, source_depth, source_camera, target_camera, target_point2d, &valid);
 
         if (K4A_RESULT_SUCCEEDED != result)
         {
             throw error("Calibration contained invalid transformation parameters!");
         }
         return static_cast<bool>(valid);
     }
 
     bool convert_color_2d_to_depth_2d(const k4a_float2_t &source_point2d,
                                       const image &depth_image,
                                       k4a_float2_t *target_point2d) const
     {
         int valid = 0;
         k4a_result_t result =
             k4a_calibration_color_2d_to_depth_2d(this, &source_point2d, depth_image.handle(), target_point2d, &valid);
 
         if (K4A_RESULT_SUCCEEDED != result)
         {
             throw error("Calibration contained invalid transformation parameters!");
         }
         return static_cast<bool>(valid);
     }
 
     static calibration get_from_raw(char *raw_calibration,
                                     size_t raw_calibration_size,
                                     k4a_depth_mode_t target_depth_mode,
                                     k4a_color_resolution_t target_color_resolution)
     {
         calibration calib;
         k4a_result_t result = k4a_calibration_get_from_raw(raw_calibration,
                                                            raw_calibration_size,
                                                            target_depth_mode,
                                                            target_color_resolution,
                                                            &calib);
 
         if (K4A_RESULT_SUCCEEDED != result)
         {
             throw error("Failed to load calibration from raw calibration blob!");
         }
         return calib;
     }
 
     static calibration get_from_raw(uint8_t *raw_calibration,
                                     size_t raw_calibration_size,
                                     k4a_depth_mode_t target_depth_mode,
                                     k4a_color_resolution_t target_color_resolution)
     {
         return get_from_raw(reinterpret_cast<char *>(raw_calibration),
                             raw_calibration_size,
                             target_depth_mode,
                             target_color_resolution);
     }
 
     static calibration get_from_raw(std::vector<uint8_t> &raw_calibration,
                                     k4a_depth_mode_t target_depth_mode,
                                     k4a_color_resolution_t target_color_resolution)
     {
         return get_from_raw(reinterpret_cast<char *>(raw_calibration.data()),
                             raw_calibration.size(),
                             target_depth_mode,
                             target_color_resolution);
     }
 };
 
 class transformation
 {
 public:
     transformation(const k4a_calibration_t &calibration) noexcept :
         m_handle(k4a_transformation_create(&calibration)),
         m_color_resolution({ calibration.color_camera_calibration.resolution_width,
                              calibration.color_camera_calibration.resolution_height }),
         m_depth_resolution({ calibration.depth_camera_calibration.resolution_width,
                              calibration.depth_camera_calibration.resolution_height })
     {
     }
 
     transformation(k4a_transformation_t handle = nullptr) noexcept : m_handle(handle) {}
 
     transformation(transformation &&other) noexcept :
         m_handle(other.m_handle),
         m_color_resolution(other.m_color_resolution),
         m_depth_resolution(other.m_depth_resolution)
     {
         other.m_handle = nullptr;
     }
 
     transformation(const transformation &) = delete;
 
     ~transformation()
     {
         destroy();
     }
 
     transformation &operator=(transformation &&other) noexcept
     {
         if (this != &other)
         {
             destroy();
             m_handle = other.m_handle;
             m_color_resolution = other.m_color_resolution;
             m_depth_resolution = other.m_depth_resolution;
             other.m_handle = nullptr;
         }
 
         return *this;
     }
 
     transformation &operator=(std::nullptr_t) noexcept
     {
         destroy();
         return *this;
     }
 
     transformation &operator=(const transformation &) = delete;
 
     void destroy() noexcept
     {
         if (m_handle != nullptr)
         {
             k4a_transformation_destroy(m_handle);
             m_handle = nullptr;
         }
     }
 
     void depth_image_to_color_camera(const image &depth_image, image *transformed_depth_image) const
     {
         k4a_result_t result = k4a_transformation_depth_image_to_color_camera(m_handle,
                                                                              depth_image.handle(),
                                                                              transformed_depth_image->handle());
         if (K4A_RESULT_SUCCEEDED != result)
         {
             throw error("Failed to convert depth map to color camera geometry!");
         }
     }
 
     image depth_image_to_color_camera(const image &depth_image) const
     {
         image transformed_depth_image = image::create(K4A_IMAGE_FORMAT_DEPTH16,
                                                       m_color_resolution.width,
                                                       m_color_resolution.height,
                                                       m_color_resolution.width *
                                                           static_cast<int32_t>(sizeof(uint16_t)));
         depth_image_to_color_camera(depth_image, &transformed_depth_image);
         return transformed_depth_image;
     }
 
     void depth_image_to_color_camera_custom(const image &depth_image,
                                             const image &custom_image,
                                             image *transformed_depth_image,
                                             image *transformed_custom_image,
                                             k4a_transformation_interpolation_type_t interpolation_type,
                                             uint32_t invalid_custom_value) const
     {
         k4a_result_t result = k4a_transformation_depth_image_to_color_camera_custom(m_handle,
                                                                                     depth_image.handle(),
                                                                                     custom_image.handle(),
                                                                                     transformed_depth_image->handle(),
                                                                                     transformed_custom_image->handle(),
                                                                                     interpolation_type,
                                                                                     invalid_custom_value);
         if (K4A_RESULT_SUCCEEDED != result)
         {
             throw error("Failed to convert depth map and custom image to color camera geometry!");
         }
     }
 
     std::pair<image, image>
     depth_image_to_color_camera_custom(const image &depth_image,
                                        const image &custom_image,
                                        k4a_transformation_interpolation_type_t interpolation_type,
                                        uint32_t invalid_custom_value) const
     {
         image transformed_depth_image = image::create(K4A_IMAGE_FORMAT_DEPTH16,
                                                       m_color_resolution.width,
                                                       m_color_resolution.height,
                                                       m_color_resolution.width *
                                                           static_cast<int32_t>(sizeof(uint16_t)));
         int32_t bytes_per_pixel;
         switch (custom_image.get_format())
         {
         case K4A_IMAGE_FORMAT_CUSTOM8:
             bytes_per_pixel = static_cast<int32_t>(sizeof(int8_t));
             break;
         case K4A_IMAGE_FORMAT_CUSTOM16:
             bytes_per_pixel = static_cast<int32_t>(sizeof(int16_t));
             break;
         default:
             throw error("Failed to support this format of custom image!");
         }
         image transformed_custom_image = image::create(custom_image.get_format(),
                                                        m_color_resolution.width,
                                                        m_color_resolution.height,
                                                        m_color_resolution.width * bytes_per_pixel);
         depth_image_to_color_camera_custom(depth_image,
                                            custom_image,
                                            &transformed_depth_image,
                                            &transformed_custom_image,
                                            interpolation_type,
                                            invalid_custom_value);
         return { std::move(transformed_depth_image), std::move(transformed_custom_image) };
     }
 
     void color_image_to_depth_camera(const image &depth_image,
                                      const image &color_image,
                                      image *transformed_color_image) const
     {
         k4a_result_t result = k4a_transformation_color_image_to_depth_camera(m_handle,
                                                                              depth_image.handle(),
                                                                              color_image.handle(),
                                                                              transformed_color_image->handle());
         if (K4A_RESULT_SUCCEEDED != result)
         {
             throw error("Failed to convert color image to depth camera geometry!");
         }
     }
 
     image color_image_to_depth_camera(const image &depth_image, const image &color_image) const
     {
         image transformed_color_image = image::create(K4A_IMAGE_FORMAT_COLOR_BGRA32,
                                                       m_depth_resolution.width,
                                                       m_depth_resolution.height,
                                                       m_depth_resolution.width * 4 *
                                                           static_cast<int32_t>(sizeof(uint8_t)));
         color_image_to_depth_camera(depth_image, color_image, &transformed_color_image);
         return transformed_color_image;
     }
 
     void depth_image_to_point_cloud(const image &depth_image, k4a_calibration_type_t camera, image *xyz_image) const
     {
         k4a_result_t result =
             k4a_transformation_depth_image_to_point_cloud(m_handle, depth_image.handle(), camera, xyz_image->handle());
         if (K4A_RESULT_SUCCEEDED != result)
         {
             throw error("Failed to transform depth image to point cloud!");
         }
     }
 
     image depth_image_to_point_cloud(const image &depth_image, k4a_calibration_type_t camera) const
     {
         image xyz_image = image::create(K4A_IMAGE_FORMAT_CUSTOM,
                                         depth_image.get_width_pixels(),
                                         depth_image.get_height_pixels(),
                                         depth_image.get_width_pixels() * 3 * static_cast<int32_t>(sizeof(int16_t)));
         depth_image_to_point_cloud(depth_image, camera, &xyz_image);
         return xyz_image;
     }
 
 private:
     k4a_transformation_t m_handle;
     struct resolution
     {
         int32_t width;
         int32_t height;
     };
     resolution m_color_resolution;
     resolution m_depth_resolution;
 };
 
 class device
 {
 public:
     device(k4a_device_t handle = nullptr) noexcept : m_handle(handle) {}
 
     device(device &&dev) noexcept : m_handle(dev.m_handle)
     {
         dev.m_handle = nullptr;
     }
 
     device(const device &) = delete;
 
     ~device()
     {
         close();
     }
 
     device &operator=(const device &) = delete;
 
     device &operator=(device &&dev) noexcept
     {
         if (this != &dev)
         {
             close();
             m_handle = dev.m_handle;
             dev.m_handle = nullptr;
         }
         return *this;
     }
 
     explicit operator bool() const noexcept
     {
         return is_valid();
     }
 
     bool is_valid() const noexcept
     {
         return m_handle != nullptr;
     }
 
     k4a_device_t handle() const noexcept
     {
         return m_handle;
     }
 
     void close() noexcept
     {
         if (m_handle != nullptr)
         {
             k4a_device_close(m_handle);
             m_handle = nullptr;
         }
     }
 
     bool get_capture(capture *cap, std::chrono::milliseconds timeout)
     {
         k4a_capture_t capture_handle = nullptr;
         int32_t timeout_ms = internal::clamp_cast<int32_t>(timeout.count());
         k4a_wait_result_t result = k4a_device_get_capture(m_handle, &capture_handle, timeout_ms);
         if (result == K4A_WAIT_RESULT_FAILED)
         {
             throw error("Failed to get capture from device!");
         }
         else if (result == K4A_WAIT_RESULT_TIMEOUT)
         {
             return false;
         }
 
         *cap = capture(capture_handle);
         return true;
     }
 
     bool get_capture(capture *cap)
     {
         return get_capture(cap, std::chrono::milliseconds(K4A_WAIT_INFINITE));
     }
 
     bool get_imu_sample(k4a_imu_sample_t *imu_sample, std::chrono::milliseconds timeout)
     {
         int32_t timeout_ms = internal::clamp_cast<int32_t>(timeout.count());
         k4a_wait_result_t result = k4a_device_get_imu_sample(m_handle, imu_sample, timeout_ms);
         if (result == K4A_WAIT_RESULT_FAILED)
         {
             throw error("Failed to get IMU sample from device!");
         }
         else if (result == K4A_WAIT_RESULT_TIMEOUT)
         {
             return false;
         }
 
         return true;
     }
 
     bool get_imu_sample(k4a_imu_sample_t *imu_sample)
     {
         return get_imu_sample(imu_sample, std::chrono::milliseconds(K4A_WAIT_INFINITE));
     }
 
     void start_cameras(const k4a_device_configuration_t *configuration)
     {
         k4a_result_t result = k4a_device_start_cameras(m_handle, configuration);
         if (K4A_RESULT_SUCCEEDED != result)
         {
             throw error("Failed to start cameras!");
         }
     }
 
     void stop_cameras() noexcept
     {
         k4a_device_stop_cameras(m_handle);
     }
 
     void start_imu()
     {
         k4a_result_t result = k4a_device_start_imu(m_handle);
         if (K4A_RESULT_SUCCEEDED != result)
         {
             throw error("Failed to start IMU!");
         }
     }
 
     void stop_imu() noexcept
     {
         k4a_device_stop_imu(m_handle);
     }
 
     std::string get_serialnum() const
     {
         std::string serialnum;
         size_t buffer = 0;
         k4a_buffer_result_t result = k4a_device_get_serialnum(m_handle, &serialnum[0], &buffer);
 
         if (result == K4A_BUFFER_RESULT_TOO_SMALL && buffer > 1)
         {
             serialnum.resize(buffer);
             result = k4a_device_get_serialnum(m_handle, &serialnum[0], &buffer);
             if (result == K4A_BUFFER_RESULT_SUCCEEDED && serialnum[buffer - 1] == 0)
             {
                 // std::string expects there to not be as null terminator at the end of its data but
                 // k4a_device_get_serialnum adds a null terminator, so we drop the last character of the string after we
                 // get the result back.
                 serialnum.resize(buffer - 1);
             }
         }
 
         if (result != K4A_BUFFER_RESULT_SUCCEEDED)
         {
             throw error("Failed to read device serial number!");
         }
 
         return serialnum;
     }
 
     void get_color_control(k4a_color_control_command_t command, k4a_color_control_mode_t *mode, int32_t *value) const
     {
         k4a_result_t result = k4a_device_get_color_control(m_handle, command, mode, value);
         if (K4A_RESULT_SUCCEEDED != result)
         {
             throw error("Failed to read color control!");
         }
     }
 
     void set_color_control(k4a_color_control_command_t command, k4a_color_control_mode_t mode, int32_t value)
     {
         k4a_result_t result = k4a_device_set_color_control(m_handle, command, mode, value);
         if (K4A_RESULT_SUCCEEDED != result)
         {
             throw error("Failed to set color control!");
         }
     }
 
     std::vector<uint8_t> get_raw_calibration() const
     {
         std::vector<uint8_t> calibration;
         size_t buffer = 0;
         k4a_buffer_result_t result = k4a_device_get_raw_calibration(m_handle, nullptr, &buffer);
 
         if (result == K4A_BUFFER_RESULT_TOO_SMALL && buffer > 1)
         {
             calibration.resize(buffer);
             result = k4a_device_get_raw_calibration(m_handle, &calibration[0], &buffer);
         }
 
         if (result != K4A_BUFFER_RESULT_SUCCEEDED)
         {
             throw error("Failed to read raw device calibration!");
         }
 
         return calibration;
     }
 
     calibration get_calibration(k4a_depth_mode_t depth_mode, k4a_color_resolution_t color_resolution) const
     {
         calibration calib;
         k4a_result_t result = k4a_device_get_calibration(m_handle, depth_mode, color_resolution, &calib);
 
         if (K4A_RESULT_SUCCEEDED != result)
         {
             throw error("Failed to read device calibration!");
         }
         return calib;
     }
 
     bool is_sync_in_connected() const
     {
         bool sync_in_jack_connected, sync_out_jack_connected;
         k4a_result_t result = k4a_device_get_sync_jack(m_handle, &sync_in_jack_connected, &sync_out_jack_connected);
 
         if (K4A_RESULT_SUCCEEDED != result)
         {
             throw error("Failed to read sync jack status!");
         }
         return sync_in_jack_connected;
     }
 
     bool is_sync_out_connected() const
     {
         bool sync_in_jack_connected, sync_out_jack_connected;
         k4a_result_t result = k4a_device_get_sync_jack(m_handle, &sync_in_jack_connected, &sync_out_jack_connected);
 
         if (K4A_RESULT_SUCCEEDED != result)
         {
             throw error("Failed to read sync jack status!");
         }
         return sync_out_jack_connected;
     }
 
     k4a_hardware_version_t get_version() const
     {
         k4a_hardware_version_t version;
         k4a_result_t result = k4a_device_get_version(m_handle, &version);
 
         if (K4A_RESULT_SUCCEEDED != result)
         {
             throw error("Failed to read device firmware information!");
         }
         return version;
     }
 
     static device open(uint32_t index)
     {
         k4a_device_t handle = nullptr;
         k4a_result_t result = k4a_device_open(index, &handle);
 
         if (K4A_RESULT_SUCCEEDED != result)
         {
             throw error("Failed to open device!");
         }
         return device(handle);
     }
 
     static uint32_t get_installed_count() noexcept
     {
         return k4a_device_get_installed_count();
     }
 
 private:
     k4a_device_t m_handle;
 };
 
 } // namespace k4a
 
 #endif