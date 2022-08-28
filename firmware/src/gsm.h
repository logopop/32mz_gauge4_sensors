
//*********************************************************************************************************************
// GSM handling
//
//*********************************************************************************************************************
//

#ifndef _GSM_H_
#define _GSM_H_ 
 
  
 
//*********************************************************************************************************************
// MCC code table
//
//*********************************************************************************************************************
//
  
/*
unsigned char ucMcc[] = 
{
   "289", "Abkhazia",
   "412", "Afghanistan",
   "276", "Albania",
   "603", "Algeria",
   "544", "American_Samoa",
   "213", "Andorra",
   "631", "Angola",
   "365", "Anguilla",
   "344", "Antigua_and_Barbuda",
   "722", "Argentina_Republic",
   "283", "Armenia",
   "363", "Aruba",
   "505", "Australia",
   "232", "Austria",
   "400", "Azerbaijan",
   "364", "Bahamas",
   "426", "Bahrain",
   "470", "Bangladesh",
   "342", "Barbados",
   "257", "Belarus",
   "206", "Belgium",
   "702", "Belize",
   "616", "Benin",
   "350", "Bermuda",
   "402", "Bhutan",
   "736", "Bolivia",
   "218", "Bosnia_&_Herzegov.",
   "652", "Botswana",
   "724", "Brazil",
   "348", "British_Virgin_Islands",
   "528", "Brunei_Darussalam",
   "284", "Bulgaria",
   "613", "Burkina_Faso",
   "642", "Burundi",
   "456", "Cambodia",
   "624", "Cameroon",
   "302", "Canada",
   "625", "Cape_Verde",
   "346", "Cayman_Islands",
   "623", "Central_African_Rep.",
   "622", "Chad",
   "730", "Chile",
   "460", "China",
   "732", "Colombia",
   "654", "Comoros",
   "630", "Congo_Dem._Rep.",
   "629", "Congo_Republic",
   "548", "Cook_Islands",
   "712", "Costa_Rica",
   "219", "Croatia",
   "368", "Cuba",
   "362", "Curacao",
   "280", "Cyprus",
   "230", "Czech_Rep.",
   "238", "Denmark",
   "638", "Djibouti",
   "366", "Dominica",
   "370", "Dominican_Republic",
   "740", "Ecuador",
   "602", "Egypt",
   "706", "El_Salvador",
   "627", "Equatorial_Guinea",
   "657", "Eritrea",
   "248", "Estonia",
   "636", "Ethiopia",
   "750", "Falkland_Islands",
   "288", "Faroe_Islands",
   "542", "Fiji",
   "244", "Finland",
   "208", "France",
   "340", "French_Guiana",
   "547", "French_Polynesia",
   "628", "Gabon",
   "607", "Gambia",
   "282", "Georgia",
   "262", "Germany",
   "620", "Ghana",
   "266", "Gibraltar",
   "202", "Greece",
   "290", "Greenland",
   "352", "Grenada",
   "340", "Guadeloupe_",
   "310", "Guam",
   "311", "Guam",
   "704", "Guatemala",
   "611", "Guinea",
   "632", "Guinea-Bissau",
   "738", "Guyana",
   "372", "Haiti",
   "708", "Honduras",
   "454", "Hongkong__China",
   "216", "Hungary",
   "274", "Iceland",
   "404", "India",
   "405", "India",
   "510", "Indonesia",
   "901", "International_Networks",
   "432", "Iran_",
   "418", "Iraq",
   "272", "Ireland",
   "425", "Israel",
   "222", "Italy",
   "612", "Ivory_Coast",
   "338", "Jamaica",
   "440", "Japan",
   "441", "Japan",
   "416", "Jordan",
   "401", "Kazakhstan",
   "639", "Kenya",
   "545", "Kiribati",
   "467", "North_Korea",
   "450", "South_Korea",
   "221", "Kosovo",
   "419", "Kuwait",
   "437", "Kyrgyzstan",
   "457", "Laos_P.D.R.",
   "247", "Latvia",
   "415", "Lebanon",
   "651", "Lesotho",
   "618", "Liberia",
   "606", "Libya",
   "295", "Liechtenstein",
   "246", "Lithuania",
   "270", "Luxembourg",
   "455", "Macao__China",
   "294", "Macedonia",
   "646", "Madagascar",
   "650", "Malawi",
   "502", "Malaysia",
   "472", "Maldives",
   "610", "Mali",
   "278", "Malta",
   "340", "Martinique",
   "609", "Mauritania",
   "617", "Mauritius",
   "334", "Mexico",
   "550", "Micronesia",
   "259", "Moldova",
   "212", "Monaco",
   "428", "Mongolia",
   "297", "Montenegro",
   "354", "Montserrat",
   "604", "Morocco",
   "643", "Mozambique",
   "414", "Myanmar_(Burma)",
   "649", "Namibia",
   "429", "Nepal",
   "204", "Netherlands",
   "362", "Netherlands_Antilles",
   "546", "New_Caledonia",
   "530", "New_Zealand",
   "710", "Nicaragua",
   "614", "Niger",
   "621", "Nigeria",
   "555", "Niue",
   "242", "Norway",
   "422", "Oman",
   "410", "Pakistan",
   "552", "Palau",
   "425", "Palestinian_Terr.",
   "714", "Panama",
   "537", "Papua_New_Guinea",
   "744", "Paraguay",
   "716", "Peru",
   "515", "Philippines",
   "260", "Poland",
   "268", "Portugal",
   "330", "Puerto_Rico",
   "427", "Qatar",
   "647", "Reunion",
   "226", "Romania",
   "250", "Russian_Federation",
   "635", "Rwanda",
   "356", "Saint_Kitts_Nevis",
   "358", "Saint_Lucia",
   "549", "Samoa",
   "292", "San_Marino",
   "626", "Sao_Tome_&_Principe",
   "901", "Satellite_Networks",
   "420", "Saudi_Arabia",
   "608", "Senegal",
   "220", "Serbia_",
   "633", "Seychelles",
   "619", "Sierra_Leone",
   "525", "Singapore",
   "231", "Slovakia",
   "293", "Slovenia",
   "540", "Solomon_Islands",
   "637", "Somalia",
   "655", "South_Africa",
   "659", "South_Sudan",
   "214", "Spain",
   "413", "Sri_Lanka",
   "308", "St._Pierre_Miquelon",
   "360", "St._Vincent_&_Gren.",
   "634", "Sudan",
   "746", "Suriname",
   "653", "Swaziland",
   "240", "Sweden",
   "228", "Switzerland",
   "417", "Syrian_Arab_Republic",
   "466", "Taiwan",
   "436", "Tajikistan",
   "640", "Tanzania",
   "520", "Thailand",
   "514", "Timor-Leste",
   "615", "Togo",
   "539", "Tonga",
   "374", "Trinidad_and_Tobago",
   "605", "Tunisia",
   "286", "Turkey",
   "438", "Turkmenistan",
   "376", "Turks_Caicos_Islands",
   "553", "Tuvalu",
   "641", "Uganda",
   "255", "Ukraine",
   "424", "United_Arab_Emirates",
   "430", "United_Arab_Emirates",
   "431", "United_Arab_Emirates",
   "234", "United_Kingdom",
   "235", "United_Kingdom",
   "310", "United_States",
   "312", "United_States",
   "311", "United_States",
   "316", "United_States",
   "748", "Uruguay",
   "434", "Uzbekistan",
   "541", "Vanuatu",
   "734", "Venezuela",
   "452", "Vietnam",
   "376", "Virgin_Islands",
   "421", "Yemen",
   "645", "Zambia",
   "648", "Zimbabwe"
}
  
*/


#endif

