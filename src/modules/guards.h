/**
 * @brief Defines the Bluetooth Low Energy (BLE) module.
 * @author Victor Miranda <varm@ic.ufal.br>
 * @date 06-11-2024
 */

#ifndef LM_GUARDS_H
#define LM_GUARDS_H

#define LM_CHECK_ERROR(cb) \
	do { \
	    int _err = cb; \
	    if (_err) { \
	        printk("Error: %d\n", _err); \
	        return _err; \
	    } \
	} while (0)

#endif /* LM_GUARDS_H */