<?php

/**
 * Helper to implementation of hook_context_default_contexts().
 */
function _wysiwyg_ckeditor_context_default_contexts() {
  $items = array();

  $items[] = array(
    'namespace' => 'rentasite',
    'attribute' => 'misc',
    'value' => 'wysiwyg_ckeditor',
    'description' => 'Pseudo context to let wysiwyg_ckeditor treat as feature',
    'block' => array(),
  );
  return $items;
}
