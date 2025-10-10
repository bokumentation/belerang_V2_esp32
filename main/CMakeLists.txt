idf_component_register(
    SRCS "main.cpp" "tb600b_so2.cpp" "uart_user_config.cpp"
    PRIV_REQUIRES my_components_cxx my_components esp_driver_uart
    INCLUDE_DIRS "." "include"
)
