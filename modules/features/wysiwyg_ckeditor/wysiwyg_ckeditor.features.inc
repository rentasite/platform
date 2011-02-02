<?php

/**
 * Implementation of hook_context_default_contexts().
 */
function wysiwyg_ckeditor_context_default_contexts() {
  module_load_include('inc', 'wysiwyg_ckeditor', 'wysiwyg_ckeditor.defaults');
  $args = func_get_args();
  return call_user_func_array('_wysiwyg_ckeditor_context_default_contexts', $args);
}
