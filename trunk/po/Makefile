PO = ru.po
MO = $(patsubst %.po,%.mo,$(PO))
POT= openfm.pot

$(MO): $(PO)

%.mo: %.po
	msgfmt -c $< -o $@

clean:
	-rm -fv $(MO) *~

update: $(PO) $(POT)
	msgmerge -U $< $(POT)

install: $(MO)
	install -pD -m 644 $< $(DESTDIR)/usr/share/locale/$(patsubst %.mo,%,$(MO))/LC_MESSAGES/openfm.mo

remove: $(MO)
	-rm -fv $(DESTDIR)/usr/share/locale/$(patsubst %.mo,%,$(MO))/LC_MESSAGES/openfm.mo

.PHONY: clean update install remove

