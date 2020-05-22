<?php
  require 'sql_pass.secret';

  $conn = mysqli_connect($sql_host, $sql_username, $sql_pass, $sql_db);

  // create a new random ID and return it to the client
  $id_is_not_unique = True;
  while ($id_is_not_unique) {
    $id = gen_id(8);
    $sql = "SELECT * FROM users_keys WHERE User_ID = '$id';";
    $result = mysqli_query($conn, $sql);

    if (mysqli_num_rows($result) == 0) {
      $id_is_not_unique = False;
    }
  }

  // add the key to do the database
  $sql = "INSERT INTO users_keys (User_ID) VALUES ('$id');";
  mysqli_query($conn, $sql);
  echo $id;

  mysqli_close($conn);

  function gen_id($length, $keyspace = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'){
    $id_arr = [];
    $max = strlen($keyspace) - 1;
    for ($i = 0; $i < $length; ++$i) {
      array_push($id_arr, $keyspace[random_int(0, $max)]);
    }
    return implode('', $id_arr);
  }
?>
