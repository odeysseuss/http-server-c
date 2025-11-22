const themeBtn=document.querySelector(".themeBtn");
const sunIcon=document.querySelector("#sun");
const moonIcon=document.querySelector("#moon");
const container=document.querySelector(".container");
const backBtn=document.querySelector(".backBtn");

const applyTheme = (theme) => {
  if (theme === 'dark') {
    document.body.classList.add("dark");
    sunIcon.style.display = "block";
    moonIcon.style.display = "none";
  } else {
    document.body.classList.remove("dark");
    sunIcon.style.display = "none";
    moonIcon.style.display = "block";
  }
}

themeBtn.addEventListener("click", () => {
  document.body.classList.toggle("dark");
  const theme = document.body.classList.contains("dark") ? "dark" : "light";
  localStorage.setItem("theme", theme);
  applyTheme(theme);
});

const storedTheme = localStorage.getItem("theme") || "light";
applyTheme(storedTheme);

backBtn.addEventListener("click",()=>{
  history.back();
})

const urlParams = new URLSearchParams(window.location.search);
const countryName = urlParams.get("name") ? decodeURIComponent(urlParams.get("name")): null;
if(countryName){
  localStorage.setItem("selectedCountryName",countryName);
}
const storedName=localStorage.getItem("selectedCountryName");
const fetchData = async () => {
  try{
    const res=await fetch("data.json");
    if(!res.ok) {
      throw new Error(`HTTP Error! Status: ${res.status}`);
    }
    const data=await res.json();
    const country=data.find(country => country.name===storedName);
    if(country) {
      displayContent(country,data);
    }else {
      document.body.innerHTML=`<h1 style="color:red">404...Oops..POST NOT FOUND!!!!!!</h1>`;
    }
  }
  catch(err){
    console.log(err);
  }
}
fetchData();

const displayContent=(country,data)=>{
  container.innerHTML=`
    <img src="${country.flags.svg}" alt="">
    <section class="country-details">
    <h1>${country.name}</h1>
    <div class="rome">
      <div>
        <p><b>Native Name: </b>${country.nativeName}</p>
        <p><b>Population: </b>${country.population}</p>
        <p><b>Region: </b>${country.region}</p>
        <p><b>Sub Region: </b>${country.subregion}</p>
        <p><b>Capital: </b>${country.capital}</p>
      </div>
      <div>
         <p><b>Top Level Domain: </b>${country.topLevelDomain.map(domain=>`<span>${domain} </span>`).join("")}</p>
         <p><b>Languages: </b>${country.languages.map(language=>`<span>${language.nativeName} </span>`).join("")}</p>
         <p><b>Currencies: </b>${country.currencies ? country.currencies.map(currency => `<span>${currency.name}</span>`).join(' ') : 'N/A'}</p>
         <p><b>Time Zones: </b>${country.timezones.map(timezone=>`<span>${timezone} </span>`).join("")}</p>
         <p><b>Area: </b>${country.area}</p>
      </div>
    </div>
    <div class="borders">
       <p><b>Border Countries: </b>
  ${country.borders && country.borders.length > 0 ? 
    country.borders.map(border => {
      const borderCountry = data.find(c => c.alpha3Code === border);
      return borderCountry ? `<a href="country.html?name=${encodeURIComponent(borderCountry.name)}">${borderCountry.name}</a>` : '';
    }).join(' ') 
  : 'None'}
</p>
    </div>   
    </section>
  `
}