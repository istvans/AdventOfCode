ifdef D
    DAY = $D
else
    $(error D=day is undefined)
endif

.PHONY: $(DAY)
$(DAY):
	cargo run --bin day${DAY}
