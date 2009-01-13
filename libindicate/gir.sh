g-ir-scanner -v --namespace Indicate --nsversion=0.1 --add-include-path=. --include=GObject-2.0 --include=GLib-2.0 --library=indicate --pkg indicate --output Indicate-0.1.gir indicator.h server.h
g-ir-compiler --includedir=. Indicate-0.1.gir -o Indicate-0.1.typelib
