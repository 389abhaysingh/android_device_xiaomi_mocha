/*
   Copyright (c) 2013, The Linux Foundation. All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>

#include "vendor_init.h"
#include "property_service.h"
#include "log.h"
#include "util.h"
#include <stdio.h>
#include <string.h>

void gsm_properties();

void vendor_load_properties()
{
    char platform[PROP_VALUE_MAX];
    char model[PROP_VALUE_MAX];
    char devicename[PROP_VALUE_MAX];
    int rc;
    FILE  *fp = NULL;
    char  *board_info = NULL;
    size_t len = 0;
    size_t read;

    rc = property_get("ro.board.platform", platform);
    if (!rc || strncmp(platform, ANDROID_TARGET, PROP_VALUE_MAX))
        return;

    // Get model from /proc/cmdline
    fp = fopen("/proc/cmdline", "r");
    if (fp == NULL)
         return;
    while ((read = getline(&board_info, &len, fp)) != (size_t)-1) {
        if (strstr(board_info, "board_info"))
            break;
    }
    fclose(fp);

    if (strstr(board_info, "0x00ea")) {
        /* Wi-Fi Only */
        property_set("ro.build.fingerprint", "nvidia/wx_na_wf/shieldtablet:5.0.1/LRX22C/29979_515.3274:user/release-keys");
        property_set("ro.build.description", "wx_na_wf-user 5.0.1 LRX22C 29979_515.3274 release-keys");
        property_set("ro.product.model", "wx_na_wf");
    } else if (strstr(board_info, "0x04d2")) {
        /* Data Only, Can't tell the difference from cmdline, so just using wx_un_do. It's the same hardware for both versions */
        gsm_properties();
        property_set("ro.build.fingerprint", "nvidia/wx_un_do/shieldtablet:5.0.1/LRX22C/29979_515.3274:user/release-keys");
        property_set("ro.build.description", "wx_un_do-user 5.0.1 LRX22C 29979_515.3274 release-keys");
        property_set("ro.product.model", "wx_un_do");
    } else { //if (strstr(board_info, "0x????"))
        /* Rest of World Voice (Device never got released, but is partially in upstream source) */
        gsm_properties();
        property_set("ro.build.fingerprint", "nvidia/wx_un_mo/shieldtablet:5.0.1/LRX22C/29979_515.3274:user/release-keys");
        property_set("ro.build.description", "wx_un_mo-user 5.0.1 LRX22C 29979_515.3274 release-keys");
        property_set("ro.product.model", "wx_un_mo");
    }

    if (board_info)
        free(board_info);

    property_set("ro.product.device", "shieldtablet");
    property_get("ro.product.model", model);
    ERROR("Setting build properties for %s model\n", model);
}

void gsm_properties()
{
    property_set("cm.icera.enabled", "1");
    property_set("ro.modem.do", "1");
    property_set("ril.icera-config-args", "notifier:ON,datastall:ON,lwaactivate");
    property_set("gsm.modem.power.device", "/sys/class/gpio/gpio147/value,0,1");
    property_set("gsm.modem.SARdet.device", "/sys/class/gpio/gpio50/value");
    property_set("gsm.modem.edp.device", "/sys/power/sysedp");
    property_set("gsm.modem.edp.state", "/sys/devices/platform/tegra_usb_modem_power/sysedp_state");
    property_set("ro.ril.devicename", "/dev/ttyACM0");
    property_set("mdc_initial_max_retry", "10");
    property_set("agpsd.socket_path", "/dev/socket/at_pal");
    property_set("agpsd.dev_path", "/dev/ttyACM4");
    property_set("rild.libpath", "libril-icera.so");
    property_set("rild.libargs", "-e rmnet0 -e rmnet0:0 -e rmnet0:1 -n");
    property_set("gsm.modem.crashlogs.directory", "/data/rfs/data/debug");
    property_set("ril.maxretries", "15");
    property_set("gsm.modem.powercontrol", "enabled");
    property_set("ro.ril.wake_lock_timeout", "200000");
    property_set("ro.telephony.default_network", "9");
}
