import { createFileRoute, Link, useNavigate } from "@tanstack/react-router";
import { useState } from "react";
import { Mail, Lock, Eye, EyeOff } from "lucide-react";

import { supabase } from "@/integrations/supabase/client";
import logoAsset from "@/assets/sentinel-logo-v4.png.asset.json";
import labAsset from "@/assets/lab-network-v4.png.asset.json";

export const Route = createFileRoute("/")({
  head: () => ({
    meta: [
      { title: "SentinelAI-X — Secure Access Sign In" },
      {
        name: "description",
        content:
          "Sign in to SentinelAI-X, the distributed laboratory security system. Detect. Analyze. Protect.",
      },
      { property: "og:title", content: "SentinelAI-X — Secure Access Sign In" },
      {
        property: "og:description",
        content: "Access your laboratory security dashboard on SentinelAI-X.",
      },
    ],
  }),
  component: SignInPage,
});

function SignInPage() {
  const navigate = useNavigate();
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  const [showPassword, setShowPassword] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [loading, setLoading] = useState(false);

  async function onSubmit(e: React.FormEvent) {
    e.preventDefault();
    setError(null);
    setLoading(true);
    const { error: signInError } = await supabase.auth.signInWithPassword({
      email: email.trim(),
      password,
    });
    setLoading(false);
    if (signInError) {
      setError("Invalid credentials. Access denied.");
      return;
    }
    navigate({ to: "/dashboard" });
  }

  return (
    <main className="relative min-h-screen overflow-hidden bg-brand-canvas">
      <div className="brand-circuit" aria-hidden="true" />

      <div className="relative mx-auto grid min-h-screen w-full max-w-[1536px] grid-cols-1 items-center gap-8 px-6 py-8 lg:grid-cols-[1fr_38%] lg:gap-6 lg:px-10">
        {/* LEFT: branding + illustration */}
        <section className="flex flex-col items-center text-center">
          <img
            src={logoAsset.url}
            alt="SentinelAI-X — Distributed Laboratory Security System"
            className="w-full max-w-[560px]"
          />
          <p className="mt-6 text-xl font-bold text-brand-navy sm:text-2xl lg:text-[30px]">
            “The Intelligence Behind Laboratory Security.”
          </p>
          <p className="mt-4 text-base font-extrabold tracking-[0.18em] sm:text-lg lg:text-[22px]">
            <span className="text-brand-blue">DETECT.</span>{" "}
            <span className="text-brand-violet">ANALYZE.</span>{" "}
            <span className="text-brand-pink">PROTECT.</span>
          </p>
          <img
            src={labAsset.url}
            alt="Distributed laboratory network with LAB 01 to LAB 04 nodes connected to a central secured server"
            className="mt-4 w-full max-w-[860px]"
          />
        </section>

        {/* RIGHT: login card */}
        <section className="mx-auto w-full max-w-[560px]">
          <div className="login-card flex flex-col justify-center px-7 py-10 sm:px-10 sm:py-14 lg:min-h-[calc(100vh-4rem)]">
            <div className="flex justify-center">
              <div className="shield-badge">
                <Lock className="h-7 w-7 text-white" strokeWidth={3} />
              </div>
            </div>

            <p className="mt-6 text-center text-lg font-bold uppercase tracking-[0.28em] text-gradient-brand">
              Secure Access
            </p>
            <div className="mx-auto mt-3 h-[2px] w-20 rounded-full bg-brand-line" />

            <h1 className="mt-4 text-center text-5xl font-bold tracking-tight text-brand-navy sm:text-[56px]">
              Sign In
            </h1>
            <p className="mt-2 text-center text-base text-brand-muted">
              Access your laboratory security dashboard
            </p>

            <form onSubmit={onSubmit} className="mt-8 space-y-5">
              <div>
                <label
                  htmlFor="email"
                  className="block text-[15px] font-bold text-brand-navy"
                >
                  Email Address
                </label>
                <div className="field mt-2">
                  <Mail className="h-5 w-5 text-brand-icon" />
                  <input
                    id="email"
                    type="email"
                    required
                    autoComplete="email"
                    maxLength={255}
                    value={email}
                    onChange={(e) => setEmail(e.target.value)}
                    placeholder="Enter your email"
                    className="field-input"
                  />
                </div>
              </div>

              <div>
                <label
                  htmlFor="password"
                  className="block text-[15px] font-bold text-brand-navy"
                >
                  Password
                </label>
                <div className="field mt-2">
                  <Lock className="h-5 w-5 text-brand-icon" />
                  <input
                    id="password"
                    type={showPassword ? "text" : "password"}
                    required
                    autoComplete="current-password"
                    maxLength={128}
                    value={password}
                    onChange={(e) => setPassword(e.target.value)}
                    placeholder="Enter your password"
                    className="field-input"
                  />
                  <button
                    type="button"
                    onClick={() => setShowPassword((v) => !v)}
                    aria-label={showPassword ? "Hide password" : "Show password"}
                    className="text-brand-icon transition-colors hover:text-brand-blue"
                  >
                    {showPassword ? (
                      <EyeOff className="h-5 w-5" />
                    ) : (
                      <Eye className="h-5 w-5" />
                    )}
                  </button>
                </div>
              </div>

              <div className="flex justify-end">
                <Link
                  to="/forgot-password"
                  className="text-[15px] font-semibold text-brand-blue hover:underline"
                >
                  Forgot Password?
                </Link>
              </div>

              {error && (
                <p className="text-center text-sm font-semibold text-destructive">
                  {error}
                </p>
              )}

              <button type="submit" disabled={loading} className="btn-brand">
                <span>{loading ? "Signing in…" : "Login"}</span>
              </button>
            </form>
          </div>
        </section>
      </div>
    </main>
  );
}
