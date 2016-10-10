module.exports = [
	{
		"type": "heading",
		"defaultValue": "Plane configuration"
	},
	{
		"type": "section",
		"items": [
			{
				"type": "radiogroup",
				"messageKey": "theme",
				"label": "Theme",
				"defaultValue": "dark",
				"options": [
					{
						"label": "Dark",
						"value": "dark"
					},
					{
						"label": "Light",
						"value": "light"
					}
				]
			},
			{
				"type": "toggle",
				"messageKey": "leading_zero",
				"label": "Hour with leading zero",
				"defaultValue": true
			},
			{
				"type": "radiogroup",
				"messageKey": "language",
				"label": "Language",
				"defaultValue": "eng",
				"options": [
					{
						"label": "English",
						"value": "eng"
					},
					{
						"label": "Spanish",
						"value": "spa"
					},
					{
						"label": "French",
						"value": "fr"
					},
					{
						"label": "German",
						"value": "ger"
					},
					{
						"label": "Italian",
						"value": "ita"
					}
				]
			},
			{
				"type": "toggle",
				"messageKey": "bt_alarm",
				"label": "Display Bluetooth alert",
				"defaultValue": false
			}
		]
	},
	{
		"type": "submit",
		"defaultValue": "Save settings"
	}
];
