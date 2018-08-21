import qbs
import qbs.TextFile

Product {
    name: "qmqtt"
    type: [
        libraryType,
        "qmqttmodule",
    ]
    property bool webSocketSupport: false
    property string libraryType: "dynamiclibrary"
    targetName: "qmqtt"

    cpp.defines: [
        "QT_BUILD_QMQTT_LIB",
        "QT_NO_CAST_TO_ASCII",
        "QT_NO_CAST_FROM_ASCII",
    ]

    cpp.includePaths: sourceDirectory

    files: [
        "*.cpp",
        "*_p.h",
    ]

    Group {
        name: "Public Headers"
        fileTags: [
            "hpp",
            "public_headers"
        ]
        files: [
            "*.h"
        ]
        excludeFiles: parent.files
    }

    Group {
        name: "QMqtt Module Template"
        fileTags: [
            "qmqttmoduletemplate"
        ]
        files: "qmqttModule.qbs"
    }

    Group {
        fileTagsFilter: libraryType
        qbs.install: true
        qbs.installDir: "lib"
    }

    Group {
        fileTagsFilter: "public_headers"
        qbs.install: true
        qbs.installDir: "include/"+product.name
    }

    Rule {
        /* TODO: copy qmqtt-module in module directory */
        inputs: "qmqttmoduletemplate"
        Artifact {
            filePath: "mqtt/"+input.fileName
            fileTags: "qmqttmodule"
        }
        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.description = "Create QMqtt Module";
            cmd.highlight = "codegen";
            cmd.sourceCode = function() {
                var file = new TextFile(input.filePath);
                var content = file.readAll();
                file.close()
                content = content.replace(/dummyIncludePath/g, '"'+input.moduleProperty("qbs", "installRoot")+'/include/'+product.targetName+'"');
                content = content.replace(/dummyLibraryPath/g, '"'+input.moduleProperty("qbs", "installRoot")+'/lib"');
                content = content.replace(/dummyLibrary/g, product.libraryType === "dynamiclibrary" ? "dynamicLibraries" : "staticLibraries");
                file = new TextFile(output.filePath, TextFile.WriteOnly);
                file.write(content);
                file.close();
            }
            return cmd;
        }
    }

    Depends {
        name: "cpp"
    }

    Depends {
        name: "Qt"
        property var baseModules: ["core", "network"]

        Properties {
            condition: webSocketSupport
            submodules: baseModules.concat(["websockets"])
        }
        Properties {
            condition: !webSocketSupport
            submodules: baseModules
        }
    }

    Export {
        Depends {
            name: "cpp"
        }

        Depends {
            name: "Qt"
            property var baseModules: ["core", "network"]
            Properties {
                condition: product.webSocketSupport
                submodules: baseModules.concat(["websockets"])
            }
            Properties {
                condition: !product.webSocketSupport
                submodules: baseModules
            }
        }

        cpp.includePaths: product.sourceDirectory
    }
}
