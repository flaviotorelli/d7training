<div class="row show-grid">
  <div class="span9" data-original-title="" title="">
    <?php print render($page['head']) ?>
  </div>
</div>
<div class="row show-grid">
   <div class="span3" data-original-title="" title=""><?php print render($page['sidebar_a']) ?></div>
   <div class="span5" data-original-title="" title="">
    <h1><?php print $title ?></h1>
    <?php print render($page['content']) ?>
  </div>
   <div class="span3" data-original-title="" title=""><?php print render($page['sidebar_b']) ?></div>
</div>
<div class="row show-grid">
  <div class="span9" data-original-title="" title="">
    <?php print render($page['footer']) ?>
  </div>
</div>