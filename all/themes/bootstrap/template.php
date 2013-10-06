<?php

function bootstrap_links__locale_block(&$vars) {
  //print '<pre>';  print_r($vars);
  foreach($vars['links'] as $language => $langInfo) {
    $abbr = $langInfo['language']->language;
    $name = $langInfo['language']->name;
    $vars['links'][$language]['title'] = '<abbr title="' . $name . '">' . $abbr . '</abbr>';
    $vars['links'][$language]['html'] = TRUE;
    
  }
  $vars['attributes']['class'][] = 'nav nav-pills';
  $content = theme_links($vars);
  return $content;
}