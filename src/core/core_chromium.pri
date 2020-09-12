qtConfig(debug_and_release): CONFIG += debug_and_release

macos {
    # This fixes namespace builds on macOS. Specifically namespace ambiguity issues between Qt and
    # Chromium forward declarations of NSString.

    # The single quotes are present so that qmake iteration does not interpret the space as
    # delimiting a new value, they are removed before writing to the GN file, and  the final shell
    # quoting is done by GN itself.
    forward_declaration_macro = "'Q_FORWARD_DECLARE_OBJC_CLASS(name)=class name;'"
} else {
    forward_declaration_macro = "Q_FORWARD_DECLARE_OBJC_CLASS=QT_FORWARD_DECLARE_CLASS"
}

# Defining keywords such as 'signal' clashes with the chromium code base.
DEFINES += QT_NO_KEYWORDS \
           QT_USE_QSTRINGBUILDER \
           $$forward_declaration_macro \
           QTWEBENGINECORE_VERSION_STR=\"$$MODULE_VERSION\" \
           QTWEBENGINEPROCESS_NAME=\"$$QTWEBENGINEPROCESS_NAME\" \
           BUILDING_CHROMIUM

# Ensure that response files, generated by qtbase/mkspecs/features/moc.prf, are found by moc.
MOC_DIR = $$OUT_PWD/$$getConfigDir()/.moc
RCC_DIR = $$OUT_PWD/$$getConfigDir()/.rcc

# Assume that we want mobile touch and low-end hardware behaviors
# whenever we are cross compiling.
qtConfig(webengine-embedded-build): DEFINES += QTWEBENGINE_EMBEDDED_SWITCHES

INCLUDEPATH += $$PWD $$PWD/api

clang_cl {
    QMAKE_CFLAGS -= $$QMAKE_CFLAGS_MSVC_COMPAT
    QMAKE_CXXFLAGS -= $$QMAKE_CFLAGS_MSVC_COMPAT
}

SOURCES = \
        accessibility_activation_observer.cpp \
        accessibility_tree_formatter_qt.cpp \
        authentication_dialog_controller.cpp \
        browser_accessibility_manager_qt.cpp \
        browser_accessibility_qt.cpp \
        browsing_data_remover_delegate_qt.cpp \
        browser_main_parts_qt.cpp \
        browser_message_filter_qt.cpp \
        certificate_error_controller.cpp \
        chromium_overrides.cpp \
        client_cert_select_controller.cpp \
        clipboard_qt.cpp \
        color_chooser_qt.cpp \
        color_chooser_controller.cpp \
        common/qt_ipc_logging.cpp \
        common/qt_messages.cpp \
        compositor/content_gpu_client_qt.cpp \
        compositor/display_frame_sink.cpp \
        compositor/display_overrides.cpp \
        compositor/display_software_output_surface.cpp \
        content_client_qt.cpp \
        content_browser_client_qt.cpp \
        content_main_delegate_qt.cpp \
        content_utility_client_qt.cpp \
        delegated_frame_host_client_qt.cpp \
        desktop_screen_qt.cpp \
        devtools_frontend_qt.cpp \
        devtools_manager_delegate_qt.cpp \
        download_manager_delegate_qt.cpp \
        favicon_manager.cpp \
        file_picker_controller.cpp \
        find_text_helper.cpp \
        javascript_dialog_controller.cpp \
        javascript_dialog_manager_qt.cpp \
        login_delegate_qt.cpp \
        media_capture_devices_dispatcher.cpp \
        native_web_keyboard_event_qt.cpp \
        net/client_cert_override.cpp \
        net/client_cert_store_data.cpp \
        net/cookie_monster_delegate_qt.cpp \
        net/custom_url_loader_factory.cpp \
        net/proxy_config_monitor.cpp \
        net/proxy_config_service_qt.cpp \
        net/proxying_url_loader_factory_qt.cpp \
        net/proxying_restricted_cookie_manager_qt.cpp \
        net/qrc_url_scheme_handler.cpp \
        net/ssl_host_state_delegate_qt.cpp \
        net/system_network_context_manager.cpp \
        net/url_request_custom_job_delegate.cpp \
        net/url_request_custom_job_proxy.cpp \
        net/webui_controller_factory_qt.cpp \
        ozone/gl_context_qt.cpp \
        ozone/gl_ozone_egl_qt.cpp \
        ozone/gl_surface_qt.cpp \
        ozone/gl_surface_egl_qt.cpp \
        ozone/gl_surface_wgl_qt.cpp \
        ozone/platform_window_qt.cpp \
        ozone/surface_factory_qt.cpp \
        permission_manager_qt.cpp \
        platform_notification_service_qt.cpp \
        process_main.cpp \
        profile_adapter.cpp \
        profile_adapter_client.cpp \
        profile_qt.cpp \
        profile_io_data_qt.cpp \
        quota_permission_context_qt.cpp \
        quota_request_controller_impl.cpp \
        pref_service_adapter.cpp \
        register_protocol_handler_request_controller_impl.cpp \
        render_view_context_menu_qt.cpp \
        render_widget_host_view_qt.cpp \
        renderer/content_renderer_client_qt.cpp \
        renderer/content_settings_observer_qt.cpp \
        renderer/render_frame_observer_qt.cpp \
        renderer/render_view_observer_qt.cpp \
        renderer/render_thread_observer_qt.cpp \
        renderer/user_resource_controller.cpp \
        renderer/plugins/plugin_placeholder_qt.cpp \
        renderer_host/render_view_observer_host_qt.cpp \
        renderer_host/user_resource_controller_host.cpp \
        resource_bundle_qt.cpp \
        resource_context_qt.cpp \
        touch_handle_drawable_qt.cpp \
        touch_selection_controller_client_qt.cpp \
        touch_selection_menu_controller.cpp \
        type_conversion.cpp \
        user_notification_controller.cpp \
        user_script.cpp \
        visited_links_manager_qt.cpp \
        web_contents_adapter.cpp \
        web_contents_delegate_qt.cpp \
        web_contents_view_qt.cpp \
        web_engine_context.cpp \
        web_engine_context_threads.cpp \
        web_engine_error.cpp \
        web_engine_library_info.cpp \
        web_engine_settings.cpp \
        web_event_factory.cpp

HEADERS = \
        accessibility_activation_observer.h \
        authentication_dialog_controller_p.h \
        authentication_dialog_controller.h \
        build_config_qt.h \
        browser_accessibility_manager_qt.h \
        browser_accessibility_qt.h \
        browsing_data_remover_delegate_qt.h \
        browser_main_parts_qt.h \
        browser_message_filter_qt.h \
        certificate_error_controller_p.h \
        certificate_error_controller.h \
        client_cert_select_controller.h \
        clipboard_change_observer.h \
        clipboard_qt.h \
        color_chooser_qt.h \
        color_chooser_controller_p.h \
        color_chooser_controller.h \
        common/qt_messages.h \
        compositor/content_gpu_client_qt.h \
        compositor/display_frame_sink.h \
        compositor/display_software_output_surface.h \
        content_client_qt.h \
        content_browser_client_qt.h \
        content_main_delegate_qt.h \
        content_utility_client_qt.h \
        delegated_frame_host_client_qt.h \
        desktop_screen_qt.h \
        devtools_frontend_qt.h \
        devtools_manager_delegate_qt.h \
        download_manager_delegate_qt.h \
        favicon_manager.h \
        file_picker_controller.h \
        find_text_helper.h \
        global_descriptors_qt.h \
        javascript_dialog_controller_p.h \
        javascript_dialog_controller.h \
        javascript_dialog_manager_qt.h \
        login_delegate_qt.h \
        media_capture_devices_dispatcher.h \
        net/client_cert_override.h \
        net/client_cert_store_data.h \
        net/cookie_monster_delegate_qt.h \
        net/custom_url_loader_factory.h \
        net/proxying_url_loader_factory_qt.h \
        net/proxying_restricted_cookie_manager_qt.h \
        net/qrc_url_scheme_handler.h \
        net/ssl_host_state_delegate_qt.h \
        net/system_network_context_manager.h \
        net/url_request_custom_job_delegate.h \
        net/url_request_custom_job_proxy.h \
        net/webui_controller_factory_qt.h \
        ozone/gl_context_qt.h \
        ozone/gl_ozone_egl_qt.h \
        ozone/gl_surface_qt.h \
        ozone/gl_surface_egl_qt.h \
        ozone/gl_surface_wgl_qt.h \
        ozone/platform_window_qt.h \
        ozone/surface_factory_qt.h \
        permission_manager_qt.h \
        platform_notification_service_qt.h \
        pref_service_adapter.h \
        process_main.h \
        profile_adapter.h \
        profile_adapter_client.h \
        profile_qt.h \
        profile_io_data_qt.h \
        proxy_config_monitor.h \
        proxy_config_service_qt.h \
        quota_permission_context_qt.h \
        quota_request_controller.h \
        quota_request_controller_impl.h \
        register_protocol_handler_request_controller.h \
        register_protocol_handler_request_controller_impl.h \
        render_view_context_menu_qt.h \
        render_widget_host_view_qt.h \
        render_widget_host_view_qt_delegate.h \
        renderer/content_renderer_client_qt.h \
        renderer/content_settings_observer_qt.h \
        renderer/render_frame_observer_qt.h \
        renderer/render_view_observer_qt.h \
        renderer/render_thread_observer_qt.h \
        renderer/user_resource_controller.h \
        renderer/plugins/plugin_placeholder_qt.h \
        renderer_host/render_view_observer_host_qt.h \
        renderer_host/user_resource_controller_host.h \
        request_controller.h \
        resource_context_qt.h \
        touch_handle_drawable_client.h \
        touch_handle_drawable_qt.h \
        touch_selection_controller_client_qt.h \
        touch_selection_menu_controller.h \
        type_conversion.h \
        user_notification_controller.h \
        user_script.h \
        visited_links_manager_qt.h \
        web_contents_adapter.h \
        web_contents_adapter_client.h \
        web_contents_delegate_qt.h \
        web_contents_view_qt.h \
        web_engine_context.h \
        web_engine_error.h \
        web_engine_library_info.h \
        web_engine_settings.h \
        web_event_factory.h

qtConfig(webengine-ozone-x11) {
    HEADERS += ozone/gl_ozone_glx_qt.h \
               ozone/gl_surface_glx_qt.h
    SOURCES += ozone/gl_surface_glx_qt.cpp \
               ozone/gl_ozone_glx_qt.cpp
}

qtConfig(webengine-pepper-plugins) {

    SOURCES += \
        renderer_host/pepper/pepper_flash_browser_host_qt.cpp \
        renderer_host/pepper/pepper_host_factory_qt.cpp \
        renderer_host/pepper/pepper_isolated_file_system_message_filter.cpp \
        renderer/pepper/pepper_flash_renderer_host_qt.cpp \
        renderer/pepper/pepper_renderer_host_factory_qt.cpp \
        renderer/plugins/loadable_plugin_placeholder_qt.cpp

    HEADERS += \
        renderer_host/pepper/pepper_flash_browser_host_qt.h \
        renderer_host/pepper/pepper_host_factory_qt.h \
        renderer_host/pepper/pepper_isolated_file_system_message_filter.h \
        renderer/pepper/pepper_flash_renderer_host_qt.h \
        renderer/pepper/pepper_renderer_host_factory_qt.h \
        renderer/plugins/loadable_plugin_placeholder_qt.h
}

qtConfig(webengine-printing-and-pdf) {

    SOURCES += \
        printing/printing_message_filter_qt.cpp \
        printing/print_view_manager_base_qt.cpp \
        printing/print_view_manager_qt.cpp \
        renderer/print_web_view_helper_delegate_qt.cpp

    HEADERS += \
        printing/printing_message_filter_qt.h \
        printing/print_view_manager_base_qt.h \
        printing/print_view_manager_qt.h \
        renderer/print_web_view_helper_delegate_qt.h

    # pdf sources
    SOURCES += printing/pdfium_document_wrapper_qt.cpp
    HEADERS += printing/pdfium_document_wrapper_qt.h
}

contains(QT_CONFIG, opengl) {
    SOURCES += \
        compositor/compositor_resource_fence.cpp \
        compositor/display_gl_output_surface.cpp \
        compositor/display_gl_output_surface_qsg.cpp
    HEADERS += \
        compositor/compositor_resource_fence.h \
        compositor/display_gl_output_surface.h
    macos {
        HEADERS+=macos_context_type_helper.h
        SOURCES+=macos_context_type_helper.mm
    }
}

qtConfig(webengine-geolocation) {
    SOURCES += location_provider_qt.cpp
    HEADERS += location_provider_qt.h
}

qtConfig(webengine-webchannel) {
    HEADERS += renderer/web_channel_ipc_transport.h \
               renderer_host/web_channel_ipc_transport_host.h

    SOURCES += renderer/web_channel_ipc_transport.cpp \
               renderer_host/web_channel_ipc_transport_host.cpp
}

qtConfig(webengine-extensions) {
    SOURCES += \
        common/extensions/extensions_api_provider_qt.cpp \
        common/extensions/extensions_client_qt.cpp \
        extensions/component_extension_resource_manager_qt.cpp \
        extensions/extension_system_qt.cpp \
        extensions/extension_system_factory_qt.cpp \
        extensions/extension_web_contents_observer_qt.cpp \
        extensions/extensions_api_client_qt.cpp \
        extensions/extensions_browser_client_qt.cpp \
        extensions/mime_handler_view_guest_delegate_qt.cpp \
        net/plugin_response_interceptor_url_loader_throttle.cpp \
        renderer/extensions/extensions_dispatcher_delegate_qt.cpp \
        renderer/extensions/extensions_renderer_client_qt.cpp \
        renderer/extensions/renderer_permissions_policy_delegate_qt.cpp \
        renderer/extensions/resource_request_policy_qt.cpp

    HEADERS += \
        common/extensions/extensions_api_provider_qt.h \
        common/extensions/extensions_client_qt.h \
        extensions/component_extension_resource_manager_qt.h \
        extensions/extension_system_qt.h \
        extensions/extension_system_factory_qt.h \
        extensions/extension_web_contents_observer_qt.h \
        extensions/extensions_api_client_qt.h \
        extensions/extensions_browser_client_qt.h \
        extensions/mime_handler_view_guest_delegate_qt.h \
        net/plugin_response_interceptor_url_loader_throttle.h \
        renderer/extensions/extensions_dispatcher_delegate_qt.h \
        renderer/extensions/extensions_renderer_client_qt.h \
        renderer/extensions/renderer_permissions_policy_delegate_qt.h \
        renderer/extensions/resource_request_policy_qt.h
}
