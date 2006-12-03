LANGUAGES = ru

TARGETS = $(addsuffix .mo,$(LANGUAGES))

$(TARGETS): %.mo: %.po
	msgfmt -c $< -o $@

all-local: $(TARGETS)

install-data-local: $(TARGETS)
	for lang in $(LANGUAGES); \
	do \
	$(mkinstalldirs) $(DESTDIR)/usr/share/locale/$$lang/LC_MESSAGES; \
	$(INSTALL_DATA) $${lang}.mo $(DESTDIR)/usr/share/locale/$$lang/LC_MESSAGES/openfm.mo; \
	done

clean-local:
	$(RM) $(TARGETS)
