target_sources(passwing PRIVATE
    callout.cpp
    callout.h
    customscrollarea.h
    datapointdialog.cpp
    datapointdialog.h
    glwidget.cpp
    glwidget.h
    imagebutton.h
    rangeslider.cpp
    rangeslider.h
    switchbutton.cpp
    switchbutton.h
)

# customscrollarea.cpp and statuslightlabel.cpp are legacy implementations;
# their active widgets are currently header-only.
