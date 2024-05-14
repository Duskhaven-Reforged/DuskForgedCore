ALTER TABLE acore_characters.item_instance DROP COLUMN enchant;
ALTER TABLE `item_instance` ADD COLUMN `enchant` INT UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE acore_characters.item_instance DROP COLUMN transmog;
ALTER TABLE `item_instance` ADD COLUMN `transmog` INT UNSIGNED NOT NULL DEFAULT '0' AFTER `text`;
