// SentinelAI-X — Secure Access sign in (standalone)
(function () {
  const form = document.getElementById("login-form");
  const email = document.getElementById("email");
  const password = document.getElementById("password");
  const toggle = document.getElementById("toggle-password");
  const eyeOpen = document.getElementById("eye-open");
  const eyeClosed = document.getElementById("eye-closed");
  const errorEl = document.getElementById("form-error");
  const btn = document.getElementById("submit-btn");
  const label = document.getElementById("submit-label");

  // Show / hide password
  toggle.addEventListener("click", function () {
    const showing = password.type === "text";
    password.type = showing ? "password" : "text";
    eyeOpen.classList.toggle("hidden", !showing);
    eyeClosed.classList.toggle("hidden", showing);
    toggle.setAttribute("aria-label", showing ? "Show password" : "Hide password");
  });

  function showError(message) {
    errorEl.textContent = message;
    errorEl.classList.remove("hidden");
  }

  function clearError() {
    errorEl.textContent = "";
    errorEl.classList.add("hidden");
  }

  form.addEventListener("submit", async function (event) {
    event.preventDefault();
    clearError();

    const emailValue = email.value.trim();
    const passwordValue = password.value;

    if (!emailValue || !/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(emailValue)) {
      showError("Enter a valid email address.");
      email.focus();
      return;
    }
    if (!passwordValue) {
      showError("Enter your password.");
      password.focus();
      return;
    }

    btn.disabled = true;
    label.textContent = "Signing in\u2026";

    try {
      // Replace this block with a real authentication request, e.g.:
      // const res = await fetch("/api/login", {
      //   method: "POST",
      //   headers: { "Content-Type": "application/json" },
      //   body: JSON.stringify({ email: emailValue, password: passwordValue }),
      // });
      // if (!res.ok) throw new Error("denied");
      // window.location.href = "/dashboard";
      await new Promise((resolve) => setTimeout(resolve, 700));
      showError("Invalid credentials. Access denied.");
    } catch (err) {
      showError("Invalid credentials. Access denied.");
    } finally {
      btn.disabled = false;
      label.textContent = "Login";
    }
  });
})();
