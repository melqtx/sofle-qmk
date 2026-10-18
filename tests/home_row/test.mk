# SPDX-License-Identifier: GPL-2.0-or-later
INTROSPECTION_KEYMAP_C = test_keymap.c
REPEAT_KEY_ENABLE = yes

ifeq ($(shell uname),Darwin)
EXTRAFLAGS += -Wno-strict-prototypes -Wno-include-next-absolute-path -Wno-c++11-narrowing
endif
