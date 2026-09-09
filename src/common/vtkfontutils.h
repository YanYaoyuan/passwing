#ifndef VTKFONTUTILS_H
#define VTKFONTUTILS_H

class vtkTextProperty;

namespace passwing {

// VTK requires a real filesystem path for custom fonts. Extract the embedded
// Chinese font to the per-user cache and configure the text property with it.
bool configureVtkChineseFont(vtkTextProperty *textProperty);

} // namespace passwing

#endif // VTKFONTUTILS_H
