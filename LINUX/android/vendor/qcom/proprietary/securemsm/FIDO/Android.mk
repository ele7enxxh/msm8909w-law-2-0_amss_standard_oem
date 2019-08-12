ifeq ($(call is-board-platform-in-list, ),true)

include $(call all-subdir-makefiles)

endif # end filter
