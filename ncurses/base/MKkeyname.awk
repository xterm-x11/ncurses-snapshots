# $Id: MKkeyname.awk,v 1.14 1998/05/10 17:00:39 tom Exp $
BEGIN {
	print "/* generated by MKkeyname.awk */"
	print ""
	print "#include <ncurses_cfg.h>"
	print "#include <stdlib.h>"
	print "#include <string.h>"
	print "#include <curses.h>"
	print ""
	print "struct kn {"
	print "\tconst char *name;"
	print "\tint code;"
	print "};"
	print ""
	print "const struct kn key_names[] = {"
}

/^[^#]/ {
#	printf "\t{ \"%s\",%*s%s },\n", $1, 16-length($1), " ", $1;
	printf "\t{ \"%s\",%s },\n", $1, $1;
	}

END {
	print "};"
	print ""
	print "NCURSES_CONST char *keyname(int c)"
	print "{"
	print "int i, size = sizeof(key_names)/sizeof(struct kn);"
	print "static char name[5];"
	print "char *p;"
	print ""
	print "\tfor (i = 0; i < size; i++)"
	print "\t\tif (key_names[i].code == c)"
	print "\t\t\treturn (NCURSES_CONST char *)key_names[i].name;"
	print "\tif (c >= 256) return \"UNKNOWN KEY\";"
	print "\tp = name;"
	print "\tif (c >= 128) {"
	print "\t\tstrcpy(p, \"M-\");"
	print "\t\tp += 2;"
	print "\t\tc -= 128;"
	print "\t}"
	print "\tif (c < 0)"
	print "\t\tsprintf(p, \"%d\", c);"
	print "\telse if (c < 32)"
	print "\t\tsprintf(p, \"^%c\", c + '@');"
	print "\telse if (c == 127)"
	print "\t\tstrcpy(p, \"^?\");"
	print "\telse"
	print "\t\tsprintf(p, \"%c\", c);"
	print "\treturn (NCURSES_CONST char *)name;"
	print "}"
	print "" 
}