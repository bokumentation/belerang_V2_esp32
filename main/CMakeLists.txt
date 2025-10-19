idf_component_register(
    SRCS "main.cpp" "data_format.cpp"
    PRIV_REQUIRES tb600b_v2 anemometer_v2
    INCLUDE_DIRS "." "include"
)
