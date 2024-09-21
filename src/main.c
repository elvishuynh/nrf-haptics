/* /Users/elvishuynh/Icarus/ncs-workspace/apollos-lyre/blinky/src/main.c */

#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <nrf/drivers/drv2605.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

/* Delay between haptic feedback in milliseconds */
#define SLEEP_TIME_MS   1000

/* Get the device tree node label for the DRV2605 device */
#define DRV2605_NODE DT_NODELABEL(drv2605_0)

/* Maximum number of attempts to check driver readiness */
#define MAX_INIT_ATTEMPTS 10

int main(void)
{
    int ret;

    LOG_INF("Starting haptic feedback application");

    /* Initialize the haptic device */
    const struct device *haptic_dev = DEVICE_DT_GET(DRV2605_NODE);
    if (!device_is_ready(haptic_dev)) {
        LOG_ERR("Haptic device not ready");
        return -EINVAL;
    }

    LOG_INF("Haptic device is ready. Waiting for driver to initialize...");

    /* Wait until the driver is ready */
    int attempts = 0;
    while (!drv2605_is_ready(haptic_dev) && attempts < MAX_INIT_ATTEMPTS) {
        LOG_DBG("Waiting for DRV2605 driver to be ready (Attempt %d/%d)", attempts + 1, MAX_INIT_ATTEMPTS);
        k_msleep(100);  // Wait 100ms before checking again
        attempts++;
    }

    if (!drv2605_is_ready(haptic_dev)) {
        LOG_ERR("DRV2605 driver failed to initialize within expected time.");
        return -EIO;
    }

    LOG_INF("DRV2605 driver is ready.");

    while (1) {
        /* Trigger haptic feedback */
        struct sensor_value val;

        /* Set the waveform effect */
        val.val1 = 0;   // Slot 0
        val.val2 = 1;   // Effect number (choose desired effect)
        ret = sensor_attr_set(haptic_dev, SENSOR_CHAN_ALL,
                              (enum sensor_attribute)DRV2605_ATTR_WAVEFORM, &val);
        if (ret < 0) {
            LOG_ERR("Failed to set waveform: %d", ret);
            // Instead of returning, continue to retry
        }

        /* End the waveform sequence */
        val.val1 = 1;   // Slot 1
        val.val2 = 0;   // End of sequence
        ret = sensor_attr_set(haptic_dev, SENSOR_CHAN_ALL,
                              (enum sensor_attribute)DRV2605_ATTR_WAVEFORM, &val);
        if (ret < 0) {
            LOG_ERR("Failed to end waveform sequence: %d", ret);
            // Continue to retry
        }

        /* Start the haptic effect */
        ret = sensor_attr_set(haptic_dev, SENSOR_CHAN_ALL,
                              (enum sensor_attribute)DRV2605_ATTR_GO, &val);
        if (ret < 0) {
            LOG_ERR("Failed to start haptic effect: %d", ret);
            // Continue to retry
        } else {
            LOG_INF("Haptic feedback triggered");
        }

        /* Wait for the specified delay before triggering again */
        k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}
