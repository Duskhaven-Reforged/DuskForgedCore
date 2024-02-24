DROP TABLE IF EXISTS `group_instance`;
CREATE TABLE `group_instance` (
  `guid` int NOT NULL,
  `instance` int NOT NULL,
  `map` int NOT NULL,
  `difficulty` int NOT NULL,
  `permanent` tinyint unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`instance`,`map`,`difficulty`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;