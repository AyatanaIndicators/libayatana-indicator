# libayatana-indicator - Ayatana Indicators Shared Library  [![Build Status](https://api.travis-ci.com/AyatanaIndicators/libayatana-indicator.svg)](https://travis-ci.com/github/AyatanaIndicators/libayatana-indicator)

## About Ayatana Indicators

The Ayatana Indicators project is the continuation of Application
Indicators and System Indicators, two technologies developed by Canonical
Ltd. for the Unity7 desktop.

Application Indicators are a GTK implementation of the StatusNotifierItem
Specification (SNI) that was originally submitted to freedesktop.org by
KDE.

System Indicators are an extensions to the Application Indicators idea.
System Indicators allow for far more widgets to be displayed in the
indicator's menu.

The Ayatana Indicators project is the new upstream for application
indicators, system indicators and associated projects with a focus on
making Ayatana Indicators a desktop agnostic technology.

On GNU/Linux, Ayatana Indicators are currently available for desktop
envinronments like MATE (used by default in [Ubuntu
MATE](https://ubuntu-mate.com)), XFCE (used by default in
[Xubuntu](https://bluesabre.org/2021/02/25/xubuntu-21-04-progress-update/),
LXDE, and the Budgie Desktop).

The Lomiri Operating Environment (UI of the Ubuntu Touch OS, formerly
known as Unity8) uses Ayatana Indicators for rendering its notification
area and the [UBports](https://ubports.com) project is a core contributor
to the Ayatana Indicators project.

For further info, please visit:
https://ayatana-indicators.org

## About this Software Component

Modern desktop panels find out about indicators by looking at indicator
service files in `/usr/share/ayatana/indicators`. These files need to have
the same name as the well-known D-Bus name that the corresponding service
owns.

An indicator file is a normal key file (like a `.desktop` file). It must have
an `[Indicator Service]` section, that must contain the service's name (`Name`)
and the object path at which its action group is found (`ObjectPath`). For
example:

```
[Indicator Service]
Name=indicator-example
ObjectPath=/org/ayatana/indicator/example
```

It should also contain a hint to where the indicator should appear in the panel:

```
Position=70
```

The lower the position, the further to the right (or left when RTL is
enabled) the indicator appears.

An indicator can only export one action group, but it supports a menu for each profile
("desktop", "greeter", "phone"). There is a section for each
of those profiles, containing the object path on which the menu is
exported:

```
[desktop]
ObjectPath=/org/ayatana/indicator/example/desktop

[greeter]
ObjectPath=/org/ayatana/indicator/example/desktop

[phone]
ObjectPath=/org/ayatana/indicator/example/phone
```

Object paths can be reused for different profiles (the greeter uses the
same menu as the desktop in the above example).

There are no fallbacks. If a profile is not mentioned in the service file,
the indicator will not show up for that profile.

## License and Copyright

See COPYING and AUTHORS file in this project.

## Building and Testing

For instructions on building and running built-in tests, see the INSTALL.md file.
