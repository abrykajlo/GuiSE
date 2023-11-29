# component definition
Component :: n number : b bool : cmpt;

# type definition: type tag at end is optional
type Vec2 : x num : y num;

# the difference between component and type is that cmpt expects that
# there is a registered type in C++ which has rendering capabilities

# function definition
fn compoundComponent : arg1 number : arg2 bool : cmpt {
    # variable definition
    x : num 4;

    # reassign variable
    x = 6;

    # using types
    v : Vec2 x 6;

    return Component x false;
    # can also use html style <Component n:x b:false />
}

# calling functions is the same as types or components
compoundComponent 30.5 false;

# this languages version of format strings
# functionStrings : fn "This is a function string {:number}";

# builtin log function
# log functionStrings 10