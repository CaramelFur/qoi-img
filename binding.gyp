{
    "targets": [
        {
            "target_name": "qoixx_addon",
            "sources": ["lib/qoi.cpp"],
            "cflags_cc": [
                "-std=c++2a -O3 -Wall -Wextra -pedantic-errors -fexceptions"
            ],
            "cflags": [
                "-O3 -Wall -Wextra -pedantic-errors -Wno-narrowing"
            ],
            "xcode_settings": {
                "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
                "CLANG_CXX_LIBRARY": "libc++",
                "CLANG_CXX_LANGUAGE_STANDARD": "c++2a"
            },
            "msvs_settings": {
                "VCCLCompilerTool": { 
                    "ExceptionHandling": 1, 
                    "AdditionalOptions": [ "-std:c++2a" ] 
                }
            }
        }
    ]
}
