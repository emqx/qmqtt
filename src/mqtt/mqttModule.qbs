import qbs

Module {
    Depends {
        name: "cpp"
    }

    Depends {
        name: "Qt"
        submodules: [
            "core",
            "network",
        ]
    }

    cpp.includePaths: dummyIncludePath
    cpp.libraryPaths: dummyLibraryPath
    cpp.dummyLibrary: "mqtt"
}
