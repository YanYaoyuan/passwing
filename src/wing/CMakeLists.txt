target_sources(passwing PRIVATE
    wingdefinition.h
    wingdisplay.cpp
    wingdisplay.h
    wingoptimization.cpp
    wingoptimization.h
    wingvlm.cpp
    wingvlm.h
)

# wingdefinition.cpp is an obsolete pre-header-only implementation and is kept
# out of the target until its duplicate constructors are removed.
