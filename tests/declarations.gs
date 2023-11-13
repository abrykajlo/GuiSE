# component definition
Component : n number : b bool : cmpt;

# type definition: type tag at end is optional
Vec2 : x number : y number : type;

# the difference between component and type is that cmpt expects that
# there is a registered type in C++ which has rendering capabilities

# function definition
compoundComponent : arg1 number : arg2 bool : fn cmpt {
    # variable definition
    x : number 4;

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