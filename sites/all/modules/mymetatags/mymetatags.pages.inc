<?php

function mymetatags_list() {

  $metatags = db_query("SELECT mtid, data, normal_path FROM {mymetatags}");
  $count = $metatags->rowCount();

  $html = '';

  if ($count == 0) {
    return t('No meta tags for you');
  }

  $html = "<ul>";

  foreach ($metatags as $key => $value) {
    if ($value) {
      $normal_path = $value->normal_path;
      $html .= "<li>". $normal_path ." - ".  drupal_get_path_alias($normal_path)  ." ".  l('+ info', 'admin/config/search/mymetatags/' . $value->mtid) ."</li>";
    }
  }

  $html .= "</ul>";
  
  return $html;
}

function mymetatags_update() {

  $metas = module_invoke_all('mymetatags_map');

  foreach ($metas as $path => $tags) {
    $normal_path = drupal_get_normal_path($path);

    foreach ($tags as $key => $tag) {
      
      // checks if the path has already been inserted
      $sql = db_query("SELECT mtid FROM {mymetatags} WHERE normal_path = :normal_path", 
        array(':normal_path' => $normal_path));

      $count_sql = $sql->rowCount();
      if ($count_sql > 0) {
        // just update the row
        db_update('mymetatags')
        ->fields(array('data' => serialize($tags)))
        ->condition('normal_path', $normal_path)
        ->execute();
      }
      else {
        // insert a new one
        db_insert('mymetatags')
        ->fields(array(
          'normal_path' => $normal_path,
          'data' => serialize($tags),
        ))
        ->execute();
      }

    }

  }
  
  return t('Meta tags has been updated.');

}

function _mymetatags_more_info($result) {

  $metas = unserialize($result->data);

  $html = '<h2>'.$result->normal_path.' ('. drupal_get_path_alias($result->normal_path) .')</h2>';

  foreach ($metas as $key => $meta) {
    $html .= "<p>$key: $meta</p>";
  }

  return $html;

}

/**
 * Meta tags update form.
 */
function mymetatags_update_form($form, $form_state, $metatagmap) {
  
  $form['normal_path_alias_html'] = array(
    '#markup' => '<h2>'. $metatagmap->normal_path . ': ' . drupal_get_path_alias($metatagmap->normal_path) . '</h2>'
  );

  //getting the tags
  $metas = unserialize($metatagmap->data);

  $validate_keys = array();

  foreach ($metas as $key => $value) {

    $form['meta_tag_' . $key] = array(
      '#type' => 'textfield',
      '#title' => $key,
      '#description' => t('Update '). $key,
      '#default_value' => $value
    );

    $validate_keys[] = $key;

  }

  $form['validate_keys'] = array(
    '#type' => 'value',
    '#value' => implode('#', $validate_keys)
  );

  $form['normal_path'] = array(
    '#type' => 'value',
    '#value' => $metatagmap->normal_path,
  ); 

  $form['mtid'] = array(
    '#type' => 'value',
    '#value' => $metatagmap->mtid,
  ); 
  
  $form['update_button'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
    '#submit' => array('mymetatags_update_form_save_submit'),
  );

  return $form;

}


function mymetatags_update_form_save_submit($form, &$form_state) {

  // checking the fields that must be validated
  $validate_fields = explode('#', $form_state['values']['validate_keys']);
  $update_fields = array();

  foreach ($validate_fields as $validate_field) {

    if (check_plain($form_state['values']['meta_tag_'. $validate_field]) != '') {
      $update_fields[] = $form_state['values']['meta_tag_'.$validate_field];
    }

  }

  // remove the register if all fields are empty
  if (count($update_fields) == 0) {
    // remove register
    //db_query("DELETE FROM {mymetatags} WHERE mtid = :mtid", array(':mtid', $form_state['values']['mtid']));
    db_delete('mymetatags')
          ->condition('mtid', $form_state['values']['mtid'])
          ->execute();

    drupal_set_message(t('Meta tags deleted'), 'status', FALSE);
    drupal_goto('admin/config/search/mymetatags/');
  }
  else {

    // rebuild "data" field and update
    $rebuild_data = array();
    foreach ($validate_fields as $key) {
      $rebuild_data[$key] = $form_state['values']['meta_tag_'.$key];
    }

    $new_data = serialize($rebuild_data);

    db_update('mymetatags')
      ->fields(array('data' => $new_data))
      ->condition('mtid', $form_state['values']['mtid'])
      ->execute();

      drupal_set_message(t('Meta tags updated'), 'status', FALSE);
      drupal_goto('admin/config/search/mymetatags/'. $form_state['values']['mtid']);

  }

}

/**
 * Meta tags insert form.
 */
/*
function mymetatags_insert_form() {

  $form = array();
  $form['ajax'] = array(
    '#markup' => '<div id="ajax">status msg</div>',
  );

  $q = check_plain($_GET['q']);

  // available meta tags
  $available_metatags = module_invoke_all('mymetatags_list');

  // checking if there is any metata for the current page
  $get_metatags = db_query("SELECT data FROM {mymetatags} WHERE normal_path = :normal_path LIMIT 1",
    array(':normal_path' => $q))->fetchField();

  $data = unserialize($get_metatags);

  foreach ($available_metatags as $key => $value) {
    $form['metatag_'. $key] = array(
      '#type' => 'textfield',
      '#title' => $key,
      '#default_value' => (!empty($data[$key]))?$data[$key]:'',
      '#description' => (empty($data[$key]))?'Insert '.$key:'Update '.$key 
    );
  }

  $form['normal_path'] = array(
    '#type' => 'value',
    '#value' => $q
  );

  $form['update_button'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array('mymetatags_insert_js_form_submit'),
    '#ajax' => array(
      'callback' => 'mymetatags_ajax_test',
      'wrapper' => 'ajax',
      'effect' => 'fade',
      )
  );

  return $form;

}

function mymetatags_ajax_test($form, $form_state) {
  return '<div id="ajax">ok</div>';
}

function mymetatags_insert_form_submit($form, &$form_state) {

  // available meta tags
  $available_metatags = module_invoke_all('mymetatags_list');
  $data = array();
  foreach ($available_metatags as $key => $description) {
    if (check_plain($form_state['values']['metatag_'.$key]) != '') {
      $data[$key] = $form_state['values']['metatag_'.$key];
    }
  }

  $data_serialized = serialize($data);

  // check if the path has some metatag already defined
  // if so, just update it
  $metatag = db_query("SELECT mtid FROM {mymetatags} WHERE normal_path = :normal_path LIMIT 1",
    array(':normal_path' => $form_state['values']['normal_path']))->fetchField();


  if ($metatag) {
    db_update('mymetatags')
      ->fields(array('data' => $data_serialized))
      ->condition('mtid', $metatag)
      ->execute();
  }
  else {  //otherwise, insert a new one
    db_insert('mymetatags')
        ->fields(array(
          'normal_path' => $form_state['values']['normal_path'],
          'data' => $data_serialized,
        ))
        ->execute();
  }

  drupal_set_message(t('Meta tags updated'), 'status', FALSE);
  drupal_goto( drupal_get_path_alias($form_state['values']['normal_path']) );

}

*/

