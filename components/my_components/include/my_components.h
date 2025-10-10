#ifndef MY_COMPONENTS_H
#define MY_COMPONENTS_H

// Required for C/C++ interoperability
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the hello_log component and prints a message.
 *
 * This function should be called once from app_main.
 */
void hello_log_init(void);

#ifdef __cplusplus
}
#endif

#endif // MY_COMPONENTS_H