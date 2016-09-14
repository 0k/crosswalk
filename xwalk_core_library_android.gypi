# Copyright (c) 2013 Intel Corporation. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'core_internal_empty_embedder_apk_name': 'XWalkCoreInternalEmptyEmbedder',
    'core_empty_embedder_apk_name': 'XWalkCoreEmptyEmbedder',
  },
  'targets': [
    {
      'target_name': 'xwalk_core_library_documentation',
      'type': 'none',
      'dependencies': [
        'xwalk_core_library_java_app_part'
      ],
      'variables': {
        'api_files': [
          '<(DEPTH)/xwalk/runtime/android/core/src/org/xwalk/core/JavascriptInterface.java',
          '<(DEPTH)/xwalk/runtime/android/core/src/org/xwalk/core/XWalkActivity.java',
          '<(DEPTH)/xwalk/runtime/android/core/src/org/xwalk/core/XWalkDialogManager.java',
          '<(DEPTH)/xwalk/runtime/android/core/src/org/xwalk/core/XWalkInitializer.java',
          '<(DEPTH)/xwalk/runtime/android/core/src/org/xwalk/core/XWalkFileChooser.java',
          '<(DEPTH)/xwalk/runtime/android/core/src/org/xwalk/core/XWalkUpdater.java',
          '<(DEPTH)/xwalk/runtime/android/core/src/org/xwalk/core/extension/XWalkExtensionContextClient.java',
          '<(DEPTH)/xwalk/runtime/android/core/src/org/xwalk/core/extension/XWalkExternalExtension.java',
          '>(reflection_gen_dir)/wrapper/org/xwalk/core/ClientCertRequest.java',
          '>(reflection_gen_dir)/wrapper/org/xwalk/core/XWalkCookieManager.java',
          '>(reflection_gen_dir)/wrapper/org/xwalk/core/XWalkDownloadListener.java',
          '>(reflection_gen_dir)/wrapper/org/xwalk/core/XWalkExternalExtensionManager.java',
          '>(reflection_gen_dir)/wrapper/org/xwalk/core/XWalkFindListener.java',
          '>(reflection_gen_dir)/wrapper/org/xwalk/core/XWalkGetBitmapCallback.java',
          '>(reflection_gen_dir)/wrapper/org/xwalk/core/XWalkHttpAuthHandler.java',
          '>(reflection_gen_dir)/wrapper/org/xwalk/core/XWalkJavascriptResult.java',
          '>(reflection_gen_dir)/wrapper/org/xwalk/core/XWalkNativeExtensionLoader.java',
          '>(reflection_gen_dir)/wrapper/org/xwalk/core/XWalkNavigationHistory.java',
          '>(reflection_gen_dir)/wrapper/org/xwalk/core/XWalkNavigationItem.java',
          '>(reflection_gen_dir)/wrapper/org/xwalk/core/XWalkPreferences.java',
          '>(reflection_gen_dir)/wrapper/org/xwalk/core/XWalkResourceClient.java',
          '>(reflection_gen_dir)/wrapper/org/xwalk/core/XWalkSettings.java',
          '>(reflection_gen_dir)/wrapper/org/xwalk/core/XWalkUIClient.java',
          '>(reflection_gen_dir)/wrapper/org/xwalk/core/XWalkView.java',
          '>(reflection_gen_dir)/wrapper/org/xwalk/core/XWalkWebResourceRequest.java',
          '>(reflection_gen_dir)/wrapper/org/xwalk/core/XWalkWebResourceResponse.java',
        ],
        'xwalk_core_jar': '<(PRODUCT_DIR)/lib.java/xwalk_core_library_java_app_part.jar',
        'docs': '<(PRODUCT_DIR)/xwalk_core_library_docs',
      },
      'actions': [
        {
          'action_name': 'javadoc_xwalk_core_library',
          'message': 'Creating documentation for XWalk Core Library',
          'inputs': [
            '>(xwalk_core_jar)',
          ],
          'outputs': [
            '<(docs)',
          ],
          'action': [
            'javadoc',
            '-quiet',
            '-XDignore.symbol.file',
            '-d', '<(docs)',
            '-classpath', '<(android_sdk)/android.jar',
            '-bootclasspath', '<(xwalk_core_jar)',
            '<@(api_files)',
          ],
        },
      ],
    },
    {
      'target_name': 'xwalk_core_internal_empty_embedder_apk',
      'type': 'none',
      'dependencies': [
        'libxwalkdummy',
        'xwalk_core_internal_java',
      ],
      'variables': {
        'apk_name': '<(core_internal_empty_embedder_apk_name)',
        'java_in_dir': 'runtime/android/core_internal_empty',
        'native_lib_target': 'libxwalkdummy',
        'additional_bundled_libs': [
          '<(PRODUCT_DIR)/lib/libxwalkcore.>(android_product_extension)',
        ],
      },
      'includes': [ '../build/java_apk.gypi' ],
      'all_dependent_settings': {
        'variables': {
          'input_jars_paths': ['<(javac_jar_path)'],
        },
      },
    },
    {
      'target_name': 'xwalk_core_empty_embedder_apk',
      'type': 'none',
      'dependencies': [
        'xwalk_core_library',
      ],
      'variables': {
        'apk_name': '<(core_empty_embedder_apk_name)',
        'java_in_dir': 'runtime/android/core_internal_empty',
      },
      'includes': [ '../build/java_apk.gypi' ],
    },
    {
      'target_name': 'xwalk_core_library_java_app_part',
      'type': 'none',
      'dependencies': [
        'xwalk_core_java',
      ],
      'variables': {
        'jar_name': '<(_target_name).jar',
        'jar_final_path': '<(PRODUCT_DIR)/lib.java/<(jar_name)',
      },
      'actions': [
        {
          'action_name': 'jars_<(_target_name)',
          'message': 'Creating <(_target_name) jar',
          'inputs': [
            'build/android/merge_jars.py',
            '>@(input_jars_paths)',
          ],
          'outputs': [
            '<(jar_final_path)',
          ],
          'action': [
            'python', 'build/android/merge_jars.py',
            '--jars=>(input_jars_paths)',
            '--output-jar=<(jar_final_path)',
          ],
        },
      ],
    },
    {
      'target_name': 'xwalk_core_library_java_library_part',
      'type': 'none',
      'dependencies': [
        'xwalk_core_internal_empty_embedder_apk',
      ],
      'variables': {
        'jar_name': '<(_target_name).jar',
        'jar_final_path': '<(PRODUCT_DIR)/lib.java/<(jar_name)',
      },
      'actions': [
        {
          'action_name': 'jars_<(_target_name)',
          'message': 'Creating <(_target_name) jar',
          'inputs': [
            'build/android/merge_jars.py',
            '>@(input_jars_paths)',
          ],
          'outputs': [
            '<(jar_final_path)',
          ],
          'action': [
            'python', 'build/android/merge_jars.py',
            '--jars=>(input_jars_paths)',
            '--output-jar=<(jar_final_path)',
          ],
        },
      ],
    },
    {
      'target_name': 'xwalk_core_library_java',
      'type': 'none',
      'dependencies': [
        'xwalk_core_library_java_app_part',
        'xwalk_core_library_java_library_part',
      ],
      'variables': {
        'jar_name': '<(_target_name).jar',
        'jar_final_path': '<(PRODUCT_DIR)/lib.java/<(jar_name)',
      },
      'actions': [
        {
          'action_name': 'jars_<(_target_name)',
          'message': 'Creating <(_target_name) jar',
          'inputs': [
            'build/android/merge_jars.py',
            '>@(input_jars_paths)',
          ],
          'outputs': [
            '<(jar_final_path)',
          ],
          'action': [
            'python', 'build/android/merge_jars.py',
            '--jars=>(input_jars_paths)',
            '--output-jar=<(jar_final_path)',
            # This argument is important for this final JAR we are creating, as
            # it validates that we are filtering out the right JARs when doing
            # the merge.
            '--validate-skipped-jars-list',
          ],
        },
      ],
    },
    {
      'target_name': 'xwalk_core_library_strip_native_libs',
      'type': 'none',
      'dependencies': [
        'libxwalkcore',
        'libxwalkdummy',
      ],
      'variables': {
        'intermediate_dir': '<(PRODUCT_DIR)/<(_target_name)',
        'ordered_libraries_file': '<(intermediate_dir)/native_libraries.json',
      },
      'direct_dependent_settings': {
        'variables': {
          'stripped_native_libraries': [
            '<(intermediate_dir)/libxwalkcore.so',
            '<(intermediate_dir)/libxwalkdummy.so',
          ],
        },
      },
      'actions': [
        {
          'variables': {
            'input_libraries': [
              '<(SHARED_LIB_DIR)/libxwalkcore.so',
              '<(SHARED_LIB_DIR)/libxwalkdummy.so',
            ],
          },
          'includes': ['../build/android/write_ordered_libraries.gypi'],
        },
        {
          'action_name': 'strip_native_libraries',
          'variables': {
            'stripped_libraries_dir': '<(intermediate_dir)',
            'stamp': '<(intermediate_dir)/stamp',
          },
          'includes': ['../build/android/strip_native_libraries.gypi'],
        },
      ],
    },
    {
      'target_name': 'xwalk_core_library',
      'type': 'none',
      'dependencies': [
        'xwalk_core_library_java',
        'xwalk_core_library_strip_native_libs',
      ],
      'actions': [
        {
          'action_name': 'generate_xwalk_core_library',
          'message': 'Generating XWalk Core Library',
          'inputs': [
            '<(DEPTH)/xwalk/build/android/common_function.py',
            '<(DEPTH)/xwalk/build/android/generate_xwalk_core_library.py',
          ],
          'outputs': [
            '<(PRODUCT_DIR)/xwalk_core_library_intermediate/always_run',
          ],
          'action': [
            'python', '<(DEPTH)/xwalk/build/android/generate_xwalk_core_library.py',
            '--abi', '<(android_app_abi)',
            '--native-libraries', '>(stripped_native_libraries)',
            '-s', '<(DEPTH)',
            '-t', '<(PRODUCT_DIR)'
          ],
        },
      ],
    },
    {
      'target_name': 'xwalk_shared_library',
      'type': 'none',
      'dependencies': [
        'xwalk_core_library_java',
      ],
      'actions': [
        {
          'action_name': 'generate_xwalk_shared_library',
          'message': 'Generating XWalk Shared Library',
          'inputs': [
            '<(DEPTH)/xwalk/build/android/common_function.py',
            '<(DEPTH)/xwalk/build/android/generate_xwalk_core_library.py',
          ],
          'outputs': [
            '<(PRODUCT_DIR)/xwalk_shared_library_intermediate/always_run',
          ],
          'action': [
            'python', '<(DEPTH)/xwalk/build/android/generate_xwalk_core_library.py',
            '-s',  '<(DEPTH)',
            '-t', '<(PRODUCT_DIR)',
            '--shared',
          ],
        },
      ],
    },
    {
      'target_name': 'xwalk_core_library_pom_gen',
      'type': 'none',
      'variables': {
        'pom_input': '<(DEPTH)/xwalk/runtime/android/maven/xwalk_core_library.pom.xml.in',
        'pom_output': '<(PRODUCT_DIR)/xwalk_core_library.pom.xml',
        'artifact_id': '<(xwalk_core_library_artifact_id)',
        'artifact_version': '<(xwalk_version)',
      },
      'includes': ['build/android/maven_pom.gypi'],
    },
    {
      'target_name': 'xwalk_shared_library_pom_gen',
      'type': 'none',
      'variables': {
        'pom_input': '<(DEPTH)/xwalk/runtime/android/maven/xwalk_shared_library.pom.xml.in',
        'pom_output': '<(PRODUCT_DIR)/xwalk_shared_library.pom.xml',
        'artifact_id': '<(xwalk_shared_library_artifact_id)',
        'artifact_version': '<(xwalk_version)',
      },
      'includes': ['build/android/maven_pom.gypi'],
    },
    {
      'target_name': 'xwalk_core_library_aar',
      'type': 'none',
      'dependencies': [
        'xwalk_core_library',
        'xwalk_core_empty_embedder_apk',
        'xwalk_core_library_pom_gen',
      ],
      'actions': [
        {
          'action_name': 'generate_xwalk_core_library_aar',
          'message': 'Generating AAR of XWalk Core Library',
          'inputs': [
            '<(DEPTH)/xwalk/build/android/common_function.py',
            '<(DEPTH)/xwalk/build/android/generate_xwalk_core_library_aar.py',
          ],
          'outputs': [
            '<(PRODUCT_DIR)/xwalk_core_library_aar_intermediate/always_run',
          ],
          'action': [
            'python', '<(DEPTH)/xwalk/build/android/generate_xwalk_core_library_aar.py',
            '-t', '<(PRODUCT_DIR)',
          ],
        },
      ],
    },
    {
      'target_name': 'xwalk_shared_library_aar',
      'type': 'none',
      'dependencies': [
        'xwalk_core_empty_embedder_apk',
        'xwalk_shared_library',
        'xwalk_shared_library_pom_gen',
      ],
      'actions': [
        {
          'action_name': 'generate_xwalk_shared_library_aar',
          'message': 'Generating AAR of XWalk Shared Library',
          'inputs': [
            '<(DEPTH)/xwalk/build/android/common_function.py',
            '<(DEPTH)/xwalk/build/android/generate_xwalk_core_library_aar.py',
          ],
          'outputs': [
            '<(PRODUCT_DIR)/xwalk_shared_library_aar_intermediate/always_run',
          ],
          'action': [
            'python', '<(DEPTH)/xwalk/build/android/generate_xwalk_core_library_aar.py',
            '-t', '<(PRODUCT_DIR)',
            '--shared',
          ],
        },
      ],
    },
  ],
}
