ifeq ($(call is-board-platform-in-list, msm8909),true)

include $(call all-subdir-makefiles)

endif # end filter
