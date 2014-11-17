
MANPAGES = auto_array.3 auto_array_delete.3 auto_array_last.3 auto_string.3 hash_table_delete.3 hash_table_get_keys.3 set.3 set_all_subsets.3  set_get_item.3  set_union.3 \
	auto_array_add.3 auto_array_get.3 auto_array_put.3 hash_table.3 hash_table_get.3 hash_table_get_remove.3 set_add_item.3 set_create.3 set_get_item_index.3 \
	auto_array_create.3 auto_array_insert.3 auto_array_remove.3 hash_table_create.3 hash_table_get_all.3 hash_table_put.3 set_add_items.3 set_delete.3 set_intersection.3 \
	auto_string_append.3 auto_string_delete.3 auto_string_create.3 auto_string_length.3

clean-docs:
	rm -rf html latex

install-docs:
	install -d  $(DESTDIR)/usr/share/man/man3
	cd man/man3/ && install -c -m 0644 $(MANPAGES) $(DESTDIR)/usr/share/man/man3/
	install -d $(DESTDIR)/usr/share/doc/softsprocket/libsscont/html
	cp -r html/* $(DESTDIR)/usr/share/doc/softsprocket/libsscont/html/
	
uninstall-docs:
	-cd $(DESTDIR)/usr/share/man/man3/ && rm $(MANPAGES)	
	-rm -rf $(DESTDIR)/usr/share/doc/softsprocket/libsscont

.PHONY: install-docs uninstall-docs clean


