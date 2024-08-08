# SPDX-License-Identifier: GPL-2.0
ccflags-y += -I$(src)/include \
             -I$(src)/include/nic \
             -I$(src)/include/mgmt \
             -I$(src)/include/chips \
             -I$(src)/sdio/include

ldflags-y += --strip-debug

mt7668s-objs := \
    chips/mt7668.o \
    common/dump.o \
    common/wlan_lib.o \
    common/wlan_oid.o \
    common/wlan_p2p.o \
    mgmt/aaa_fsm.o \
    mgmt/ais_fsm.o \
    mgmt/assoc.o \
    mgmt/auth.o \
    mgmt/bss.o \
    mgmt/cnm.o \
    mgmt/cnm_mem.o \
    mgmt/cnm_timer.o \
    mgmt/hem_mbox.o \
    mgmt/mib.o \
    mgmt/p2p_assoc.o \
    mgmt/p2p_dev_fsm.o \
    mgmt/p2p_dev_state.o \
    mgmt/p2p_fsm.o \
    mgmt/p2p_func.o \
    mgmt/p2p_ie.o \
    mgmt/p2p_rlm.o \
    mgmt/p2p_rlm_obss.o \
    mgmt/p2p_role_fsm.o \
    mgmt/p2p_role_state.o \
    mgmt/p2p_scan.o \
    mgmt/privacy.o \
    mgmt/rate.o \
    mgmt/reg_rule.o \
    mgmt/rlm.o \
    mgmt/rlm_domain.o \
    mgmt/rlm_obss.o \
    mgmt/roaming_fsm.o \
    mgmt/rsn.o \
    mgmt/saa_fsm.o \
    mgmt/scan.o \
    mgmt/scan_fsm.o \
    mgmt/swcr.o \
    mgmt/tdls.o \
    mgmt/tkip_mic.o \
    mgmt/wnm.o \
    nic/cmd_buf.o \
    nic/nic.o \
    nic/nic_cmd_event.o \
    nic/nic_pwr_mgt.o \
    nic/nic_rate.o \
    nic/nic_rx.o \
    nic/nic_tx.o \
    nic/nic_umac.o \
    nic/que_mgt.o \
    sdio/gl_ate_agent.o \
    sdio/gl_cfg80211.o \
    sdio/gl_hook_api.o \
    sdio/gl_init.o \
    sdio/gl_kal.o \
    sdio/gl_qa_agent.o \
    sdio/gl_p2p.o \
    sdio/gl_p2p_cfg80211.o \
    sdio/gl_p2p_init.o \
    sdio/gl_p2p_kal.o \
    sdio/gl_proc.o \
    sdio/gl_vendor.o \
    sdio/gl_wext.o \
    sdio/gl_wext_priv.o \
    sdio/hal_api.o \
    sdio/platform.o \
    sdio/sdio.o

ifeq ($(CONFIG_MT7668S_WIFI_PREALLOC), y)
    ccflags-y += -DCFG_PREALLOC_MEMORY \
                 -I$(src)/prealloc/include
    mt7668s_prealloc-objs := prealloc/prealloc.o
    obj-$(CONFIG_MT7668S_WIFI) += mt7668s_prealloc.o
endif

ifeq ($(CONFIG_MT7668S_WIFI_MESON_G12A_PATCH),y)
    ccflags-y += -DCFG_MESON_G12A_PATCH=1
else
    ccflags-y += -DCFG_MESON_G12A_PATCH=0
endif

obj-$(CONFIG_MT7668S_WIFI) := mt7668s.o
obj-y += reinit/

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
