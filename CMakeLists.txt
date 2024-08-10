idf_component_register(SRCS
		       "src/esp32wmbusmeters.cc"
		       "src/esp32util.cc"
		       "src/esp32dynamic.cc"
		       "src/esp32threads.cc"
		       "src/esp32bus.cc"
		       "src/wmbus.cc"
		       "src/meters.cc"
		       "src/drivers.cc"
		       "src/metermanager.cc"
		       "src/address.cc"
		       "src/dvparser.cc"
		       "src/aes.cc"
		       "src/aescmac.cc"
		       "src/sha256.cc"
		       "src/wmbus_utils.cc"
		       "src/translatebits.cc"
		       "src/formula.cc"
		       "src/units.cc"
		       "src/manufacturer_specificities.cc"
	               "src/driver_flowiq2200.cc"
                       INCLUDE_DIRS "src"
		       WHOLE_ARCHIVE
		       REQUIRES driver mqtt)


