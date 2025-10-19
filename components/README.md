# Example of ESP-IDF Components

Create `components` folder in your project. Then copy this folder into your project `components` folder. Then modify the `main/CMakeList.txt` look like this.

```cmake
idf_component_register(
    SRCS "main.cpp"
    REQUIRES cxx
    PRIV_REQUIRES my_components_cxx my_components
    INCLUDE_DIRS "." 
)
```