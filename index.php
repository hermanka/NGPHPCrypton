<?php

function hello(){
  echo "hello";
}


$arr = get_defined_functions();

hello();
// print_r($arr);

if (in_array("get_defined_functions", $arr['internal']))
  {
  echo "arr Match found";
  }
else
  {
  echo "arr Match not found";
  }
?>