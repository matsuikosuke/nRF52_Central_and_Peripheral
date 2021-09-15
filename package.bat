REM set your sdk path
set SDK=D:\BLE\nRF5_SDK_17.0.2_d674dde\examples\ble_central_and_peripheral\test_project

REM NRF52840
REM set FAMILY=NRF52840
REM set HWVERSION=52
REM set SDVERSION=0x0100
REM set SOFTDEVICE=D:\BLE\nRF5_SDK_17.0.2_d674dde\examples\ble_central_and_peripheral\test_project\softdevice\s140_nrf52_7.2.0_softdevice.hex

REM nRF52832
set FAMILY=NRF52
set HWVERSION=52
set SDVERSION=0x0101
set SOFTDEVICE=D:\BLE\nRF5_SDK_17.0.2_d674dde\examples\ble_central_and_peripheral\test_project\softdevice\s132_nrf52_7.2.0_softdevice.hex

REM set your app/bl path (nRF52840)
REM set APP=D:\BLE\nRF5_SDK_17.0.2_d674dde\examples\ble_central_and_peripheral\test_project\pca10056\s140\ses\Output\Release\Exe\test3rd01_s140.hex
REM set BOOTLOADER=.\secure_bootloader\pca10056_ble\ses\Output\Release\Exe\secure_bootloader_ble_s140_pca10056.hex

REM set your app/bl path (nRF52832)
set APP=D:\BLE\nRF5_SDK_17.0.2_d674dde\examples\ble_central_and_peripheral\test_project\pca10040\s132\ses\Output\Release\Exe\test_s132.hex
set BOOTLOADER=D:\BLE\nRF5_SDK_17.0.2_d674dde\examples\ble_central_and_peripheral\test_project\secure_bootloader\pca10040_s132_ble\ses\Output\Release\Exe\secure_bootloader_ble_s132_pca10040.hex

REM set your private key path
set PRIVATEKEY=D:\BLE\nRF5_SDK_17.0.2_d674dde\examples\ble_central_and_peripheral\test_project\private.key


REM make image for ROM FLASHING
mkdir SDK\Output

nrfutil settings generate --family NRF52 --application SDK\pca10040\s132\ses\Output\Release\Exe\test_s132.hex --application-version 1 --bootloader-version 1 --bl-settings-version 1 SDK\Output\settings.hex 

mergehex --merge SOFTDEVICE BOOTLOADER --output SDK\Output\SD_BL.hex

mergehex --merge SDK\Output\SD_BL.hex SDK\pca10040\s132\ses\Output\Release\Exe\test_s132.hex --output SDK\Output\SD_BL_APP.hex

mergehex --merge SDK\Output\SD_BL_APP.hex SDK\Output\settings.hex --output SDK\Output\all.hex

REM del SDK\Output\SD_BL.hex
REM del SDK\Outputut\SD_BL_APP.hex
REM del SDK\Output\settings.hex

REM make image for OTA
nrfutil pkg generate --hw-version HWVERSION --application-version 1 --application APP --sd-req SDVERSION --key-file PRIVATEKEY SDK\Output\test_app.zip

nrfutil pkg generate --hw-version HWVERSION --bootloader-version 2 --bootloader BOOTLOADER --application-version 1 --application APP --softdevice SOFTDEVICE --sd-id SDVERSION --sd-req SDVERSION --key-file PRIVATEKEY SDK\Output\test_all.zip

nrfutil pkg generate --hw-version HWVERSION --bootloader-version 2 --bootloader BOOTLOADER --softdevice SOFTDEVICE --sd-id SOFTDEVICE --sd-req SOFTDEVICE --key-file  PRIVATEKEY SDK\Output\test_sdk.zip
