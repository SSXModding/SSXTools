// This file lists off all types that are needed by an X-macro TYPE() macro.
// These types use the same exact boilerplate code, which can be shared inside of an expansion of
// of the given TYPE() macro seen here, essentially only writing the code once and having the 
// preprocessor do the work of writing it all over again. 
//
// Kind of hampers debuggability, however,
// in the case of IoStreamReadStream, it calls into a template which can be broken into if required.

// This file can also be used to pre-instantiate some templates to make them basically free,
// as (ab)used in the ssxtools core library.

// General order:
//
// TYPE(FunctionName, TypeName) 
// is a macro expected to be defined
// at the time of inclusion of this file.
//
// FunctionName is the name of the function for this type.
// TypeName     is the name of the actual C++ backing data type.
//
//  FunctionName     TypeName

TYPE(Uint16,         std::uint16_t)
TYPE(Int16,          std::int16_t)
TYPE(Uint32,         std::uint32_t)
TYPE(Int32,          std::int32_t)
TYPE(Uint64,         std::uint64_t)
TYPE(Int64,          std::int64_t)

