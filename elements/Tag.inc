<?php

// $Id$

/**
 * @file
 *
 *  Allows forms to dynamically add new tabs to a tabpanel in a form.
 */
module_load_include('inc', 'xml_form_elements', 'Element');

/**
 * A collection of static functions that allow for theming and processing tabpanels.
 */
class Tag {
  /**
   * Constants
   */
  const RemoveButton = 'remove-tag';
  const EditButton = 'edit-tag';

  /**
   * TabPanel's theme hook.
   * 
   * @param array $element
   * @return string 
   */
  public static function Theme($element) {
    $children = isset($element['#children']) ? $element['#children'] : '';
    $description = isset($element['#description']) ? "<div class='description'>{$element['#description']}</div>" : '';
    return "<div id='{$element['#hash']}'>{$description}{$children}</div>";
  }

  /**
   * The default #process function for tabpanel's.
   * 
   * Adds elements that allow for adding/remove form elements.
   * 
   * @param array $element
   * @param mixed $edit
   * @param array $form_state
   * @param array $complete_form
   * @return array 
   */
  public static function Process(array $element, $edit, array &$form_state, array $complete_form = NULL) {
    $element['#id'] = $element['#hash'];
    $element[self::EditButton] = self::createEditButton($element, $complete_form);
    $element[self::RemoveButton] = self::createRemoveButton($element, $complete_form);
    return $element;
  }

  private static function createRemoveButton(array &$element, array &$complete_form) {
    $tags = get_form_element_parent($element, $complete_form);
    $add['#type'] = 'submit';
    $add['#weight'] = 4;
    $add['#size'] = 30;
    $add['#attributes'] = array('style' => 'display:none');
    $add['#ahah'] = array(
      'path' => "xml/form/elements/ahah/tag/remove/{$element['#hash']}",
      'wrapper' => $tags['#hash'], // The parents wrapper is set to the parents hash.
      'method' => 'replace',
      'effect' => 'fade'
    );
    return $add;
  }

  private static function createEditButton(array &$element, array &$complete_form) {
    $tags = get_form_element_parent($element, $complete_form);
    $add['#type'] = 'submit';
    $add['#weight'] = 4;
    $add['#size'] = 30;
    $add['#attributes'] = array('style' => 'display:none');
    $add['#ahah'] = array(
      'path' => "xml/form/elements/ahah/tag/edit/{$element['#hash']}",
      'wrapper' => $tags['#hash'], // The parents wrapper is set to the parents hash.
      'method' => 'replace',
      'effect' => 'fade'
    );
    return $add;
  }

  /**
   *
   * @param hash $textfield_hash
   */
  public static function Add($tags) {
    list($form_id, $form_build_id, $drupal_form, $form_state) = Ahah::getFormInfo();
    $drupal_form = Ahah::processForm($form_id, $drupal_form, $form_state); // Process the submitted form.
    $tags = find_element($drupal_form, $tags); // Find the tags we are dealing with.
    $children = get_element_children($tags, 'tag');
    $input = &$tags[array_shift($children)]; // Get the input field.
    $input_hash = $input['#hash'];
    $input_value = $input['#value'];
    form_set_value($input, '', $form_state); // Set input field to ''.
    // Duplicate Element, set its default value to the submitted value.
    $form = new XMLForm($form_state);
    $input = $form->findElement($input_hash);
    $new_element = $form->duplicateOriginal($input_hash);
    $new_element['#default_value'] = $input_value; // Get the value from input.
    $input->parent[] = $new_element; // Add to the form.
    // Rebuild.
    $drupal_form = Ahah::rebuildForm($form_id, $form_build_id, $drupal_form, $form_state);
    // Find the parent element to render.
    $element = find_element($drupal_form, $tags['#hash']);
    // Respond to the request with JSON.
    Ahah::respond($element);
  }

  /**
   *
   * @param hash $textfield_hash
   * @param hash $tag_hash 
   */
  public static function Edit($hash) {
    list($form_id, $form_build_id, $drupal_form, $form_state) = Ahah::getFormInfo();
    $drupal_form = Ahah::processForm($form_id, $drupal_form, $form_state); // Process the submitted form.
    $edit = find_element($drupal_form, $hash); // Find the tags we are dealing with.
    $tags = get_form_element_parent($edit, $drupal_form);
    $children = get_element_children($tags, 'tag');
    $input = &$tags[array_shift($children)]; // Get the input field.
    form_set_value($input, $edit['#value'], $form_state); // Set input field to ''.
    $drupal_form = Ahah::rebuildForm($form_id, $form_build_id, $drupal_form, $form_state);
    // Find the parent element to render.
    $element = find_element($drupal_form, $tags['#hash']);
    // Respond to the request with JSON.
    Ahah::respond($element);
  }

  /**
   *
   * @param hash $tag_hash 
   */
  public static function Remove($tag) {
    list($form_id, $form_build_id, $drupal_form, $form_state) = Ahah::getFormInfo();
    // Duplicate Element, set its default value to the submitted value.
    $form = new XMLForm($form_state);
    $tag = $form->findElement($tag);
    $render = $tag->parent->hash;
    $form->remove($tag->hash);
    // Process and Rebuild.
    $drupal_form = Ahah::processForm($form_id, $drupal_form, $form_state); // Process the submitted form.
    $drupal_form = Ahah::rebuildForm($form_id, $form_build_id, $drupal_form, $form_state);
    // Find the parent element to render.
    $element = find_element($drupal_form, $render);
    // Respond to the request with JSON.
    Ahah::respond($element);
  }

}

/**
 * Theme hook for a tabpanel
 */
function theme_tag($element) {
  return Tag::Theme($element);
}